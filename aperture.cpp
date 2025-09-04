#include "aperture.hpp"
#include <charconv>
#include <algorithm>
#include <cctype>
#include <sstream>

namespace secure {

// Parser Implementation
void Parser::skip_whitespace() {
    while (pos < input.size() && std::isspace(input[pos])) {
        pos++;
    }
}

bool Parser::consume(char c) {
    skip_whitespace();
    if (peek(c)) {
        pos++;
        return true;
    }
    return false;
}

std::expected<ValuePtr, Error> Parser::parse() {
    skip_whitespace();
    if (at_end()) {
        return nil();
    }
    return parse_value();
}

std::expected<ValuePtr, Error> Parser::parse_value() {
    skip_whitespace();
    
    if (at_end()) {
        return std::unexpected(Error{ErrorKind::PARSE_ERROR, "Unexpected end of input", pos});
    }
    
    if (peek('(')) {
        return parse_list();
    }
    
    return parse_atom();
}

std::expected<ValuePtr, Error> Parser::parse_list() {
    if (!consume('(')) {
        return std::unexpected(Error{ErrorKind::PARSE_ERROR, "Expected '('", pos});
    }
    
    ValueList items;
    
    while (!peek(')') && !at_end()) {
        auto val = parse_value();
        if (!val) return val;
        items.push_back(*val);
        skip_whitespace();
    }
    
    if (!consume(')')) {
        return std::unexpected(Error{ErrorKind::PARSE_ERROR, "Expected ')'", pos});
    }
    
    return list(std::move(items));
}

std::expected<ValuePtr, Error> Parser::parse_atom() {
    skip_whitespace();
    
    // Check for quote syntactic sugar: 'expr
    if (peek('\'')) {
        pos++;
        auto quoted = parse_value();
        if (!quoted) return quoted;
        // Transform 'expr into (quote expr)
        return list({sym("quote"), *quoted});
    }
    
    // Check for quasiquote syntactic sugar: `expr
    if (peek('`')) {
        pos++;
        auto quoted = parse_value();
        if (!quoted) return quoted;
        // Transform `expr into (quasiquote expr)
        return list({sym("quasiquote"), *quoted});
    }
    
    // Check for unquote syntactic sugar: ,expr
    if (peek(',')) {
        pos++;
        if (peek('@')) {
            pos++;
            auto unquoted = parse_value();
            if (!unquoted) return unquoted;
            // Transform ,@expr into (unquote-splicing expr)
            return list({sym("unquote-splicing"), *unquoted});
        }
        auto unquoted = parse_value();
        if (!unquoted) return unquoted;
        // Transform ,expr into (unquote expr)
        return list({sym("unquote"), *unquoted});
    }
    
    // Check for hole syntax: ?variable or ?{variable:constraint}
    if (peek('?')) {
        pos++;
        if (peek('{')) {
            pos++;
            std::string hole_spec;
            while (!peek('}') && !at_end()) {
                hole_spec += input[pos++];
            }
            if (!consume('}')) {
                return std::unexpected(Error{ErrorKind::PARSE_ERROR, "Expected '}'", pos});
            }
            // Parse hole_spec for id and constraint
            size_t colon = hole_spec.find(':');
            if (colon != std::string::npos) {
                auto h = hole(hole_spec.substr(0, colon));
                if (auto* hole_data = std::get_if<Value::Hole>(&h->data)) {
                    hole_data->constraint = hole_spec.substr(colon + 1);
                }
                return h;
            }
            return hole(hole_spec);
        } else {
            auto sym_result = parse_symbol();
            if (!sym_result) return std::unexpected(sym_result.error());
            return hole(*sym_result);
        }
    }
    
    // String
    if (peek('"')) {
        return parse_string().transform([](auto&& s) { return str(std::move(s)); });
    }
    
    // Number
    if (std::isdigit(current()) || (current() == '-' && pos + 1 < input.size() && std::isdigit(input[pos + 1]))) {
        return parse_number().transform([](double n) { return num(n); });
    }
    
    // Symbol
    return parse_symbol().transform([](auto&& s) { 
        // Special symbols
        if (s == "nil") return nil();
        if (s == "lambda") return sym("lambda");
        if (s == "if") return sym("if");
        if (s == "let") return sym("let");
        if (s == "quote") return sym("quote");
        return sym(std::move(s));
    });
}

std::expected<double, Error> Parser::parse_number() {
    size_t start = pos;
    
    if (current() == '-' || current() == '+') pos++;
    
    // Parse mantissa
    while (pos < input.size() && std::isdigit(input[pos])) {
        pos++;
    }
    
    if (pos < input.size() && input[pos] == '.') {
        pos++;
        while (pos < input.size() && std::isdigit(input[pos])) {
            pos++;
        }
    }
    
    // Parse exponent (scientific notation)
    if (pos < input.size() && (input[pos] == 'e' || input[pos] == 'E')) {
        pos++;
        if (pos < input.size() && (input[pos] == '+' || input[pos] == '-')) {
            pos++;
        }
        while (pos < input.size() && std::isdigit(input[pos])) {
            pos++;
        }
    }
    
    double result;
    auto [ptr, ec] = std::from_chars(input.data() + start, input.data() + pos, result, 
                                     std::chars_format::general);
    
    if (ec != std::errc()) {
        return std::unexpected(Error{ErrorKind::PARSE_ERROR, "Invalid number", start});
    }
    
    return result;
}

std::expected<std::string, Error> Parser::parse_string() {
    if (!consume('"')) {
        return std::unexpected(Error{ErrorKind::PARSE_ERROR, "Expected '\"'", pos});
    }
    
    std::string result;
    while (!peek('"') && !at_end()) {
        if (peek('\\') && pos + 1 < input.size()) {
            pos++;
            char next = input[pos++];
            switch (next) {
                case 'n': result += '\n'; break;
                case 't': result += '\t'; break;
                case 'r': result += '\r'; break;
                case '"': result += '"'; break;
                case '\\': result += '\\'; break;
                default: result += next;
            }
        } else {
            result += input[pos++];
        }
    }
    
    if (!consume('"')) {
        return std::unexpected(Error{ErrorKind::PARSE_ERROR, "Unclosed string", pos});
    }
    
    return result;
}

std::expected<std::string, Error> Parser::parse_symbol() {
    size_t start = pos;
    
    while (pos < input.size() && !std::isspace(input[pos]) && 
           input[pos] != '(' && input[pos] != ')' && input[pos] != '"') {
        pos++;
    }
    
    if (pos == start) {
        return std::unexpected(Error{ErrorKind::PARSE_ERROR, "Empty symbol", pos});
    }
    
    return std::string(input.substr(start, pos - start));
}

// Evaluator Implementation
Evaluator::Evaluator() {
    // Initialize with built-in functions
    globals["+"] = sym("+");
    globals["-"] = sym("-");
    globals["*"] = sym("*");
    globals["/"] = sym("/");
    globals["="] = sym("=");
    globals["<"] = sym("<");
    globals[">"] = sym(">");
    
    push_frame(); // Global frame
}

std::expected<ValuePtr, Error> Evaluator::eval(ValuePtr expr, bool allow_holes) {
    if (!expr) return nil();
    
    // Check complexity limit
    if (expr->meta.complexity > max_complexity) {
        return std::unexpected(Error{ErrorKind::SECURITY_VIOLATION, "Complexity limit exceeded"});
    }
    
    // Check stack depth
    if (call_stack.size() > max_depth) {
        return std::unexpected(Error{ErrorKind::SECURITY_VIOLATION, "Stack depth limit exceeded"});
    }
    
    return std::visit([this, allow_holes, expr](auto&& arg) -> std::expected<ValuePtr, Error> {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, Value::Nil>) {
            return nil();
        } else if constexpr (std::is_same_v<T, Value::Hole>) {
            if (!allow_holes) {
                return std::unexpected(Error{ErrorKind::HOLE_ACCESS, 
                    "Cannot evaluate expression with hole: " + arg.id});
            }
            return hole(arg.id, arg.sealed);
        } else if constexpr (std::is_same_v<T, Value::Num>) {
            return num(arg.val);
        } else if constexpr (std::is_same_v<T, Value::Str>) {
            return str(arg.val);
        } else if constexpr (std::is_same_v<T, Value::Sym>) {
            return eval_symbol(arg);
        } else if constexpr (std::is_same_v<T, Value::List>) {
            return eval_list(arg, allow_holes);
        } else if constexpr (std::is_same_v<T, Value::Lambda>) {
            // Lambda evaluates to itself (with captured closure)
            auto lam = std::make_shared<Value>(expr->data);
            return lam;
        }
    }, expr->data);
}

std::expected<ValuePtr, Error> Evaluator::eval_list(const Value::List& lst, bool allow_holes) {
    if (lst.items.empty()) {
        return nil();  // Empty list evaluates to nil, not an empty list
    }
    
    auto first = lst.items[0];
    
    // Check for special forms
    if (auto* sym = std::get_if<Value::Sym>(&first->data)) {
        if (sym->name == "quote") {
            // (quote expr) - Return expression unevaluated
            if (lst.items.size() != 2) {
                return std::unexpected(Error{ErrorKind::EVAL_ERROR, "Quote requires exactly one argument"});
            }
            return lst.items[1];  // Return unevaluated
        }
        else if (sym->name == "quasiquote") {
            // (quasiquote expr) - Template with unquote support
            if (lst.items.size() != 2) {
                return std::unexpected(Error{ErrorKind::EVAL_ERROR, "Quasiquote requires exactly one argument"});
            }
            return eval_quasiquote(lst.items[1], allow_holes);
        }
        else if (sym->name == "define") {
            // (define name value) or (define (name params...) body)
            if (lst.items.size() < 3) {
                return std::unexpected(Error{ErrorKind::EVAL_ERROR, "Define requires name and value"});
            }
            
            auto first_item = lst.items[1];
            
            // Check if it's a function definition: (define (name params...) body)
            if (auto* fn_list = std::get_if<Value::List>(&first_item->data)) {
                if (fn_list->items.empty()) {
                    return std::unexpected(Error{ErrorKind::EVAL_ERROR, "Define function requires name"});
                }
                
                auto* fn_name = std::get_if<Value::Sym>(&fn_list->items[0]->data);
                if (!fn_name) {
                    return std::unexpected(Error{ErrorKind::TYPE_ERROR, "Function name must be symbol"});
                }
                
                // Convert to lambda
                ValueList params(fn_list->items.begin() + 1, fn_list->items.end());
                Value::Lambda lam;
                lam.params = params;
                lam.body = lst.items[2];
                lam.closure = current_frame().bindings;
                
                auto lambda_val = std::make_shared<Value>(lam);
                globals[fn_name->name] = lambda_val;
                return lambda_val;
            }
            
            // Simple variable definition: (define name value)
            auto* name_sym = std::get_if<Value::Sym>(&first_item->data);
            if (!name_sym) {
                return std::unexpected(Error{ErrorKind::TYPE_ERROR, "Define name must be symbol"});
            }
            
            auto value = eval(lst.items[2], allow_holes);
            if (!value) return value;
            
            globals[name_sym->name] = *value;
            return *value;
        }
        else if (sym->name == "begin") {
            // (begin expr1 expr2 ...) - Evaluate expressions in sequence, return last
            if (lst.items.size() < 2) {
                return nil();
            }
            
            ValuePtr result = nil();
            for (size_t i = 1; i < lst.items.size(); i++) {
                auto val = eval(lst.items[i], allow_holes);
                if (!val) return val;
                result = *val;
            }
            return result;
        }
        else if (sym->name == "cond") {
            // (cond (test1 expr1) (test2 expr2) ... (else expr))
            if (lst.items.size() < 2) {
                return std::unexpected(Error{ErrorKind::EVAL_ERROR, "Cond requires at least one clause"});
            }
            
            for (size_t i = 1; i < lst.items.size(); i++) {
                auto* clause = std::get_if<Value::List>(&lst.items[i]->data);
                if (!clause || clause->items.size() != 2) {
                    return std::unexpected(Error{ErrorKind::TYPE_ERROR, "Cond clause must be (test expr)"});
                }
                
                // Check for else clause
                if (auto* test_sym = std::get_if<Value::Sym>(&clause->items[0]->data)) {
                    if (test_sym->name == "else") {
                        return eval(clause->items[1], allow_holes);
                    }
                }
                
                // Evaluate test
                auto test_result = eval(clause->items[0], allow_holes);
                if (!test_result) return test_result;
                
                // Check if test has holes
                if ((*test_result)->is_hole()) {
                    if (!allow_holes) {
                        return std::unexpected(Error{ErrorKind::HOLE_ACCESS, "Cannot evaluate cond with hole in test"});
                    }
                    // Return simplified cond with remaining clauses
                    ValueList remaining;
                    remaining.push_back(secure::sym("cond"));
                    for (size_t j = i; j < lst.items.size(); j++) {
                        remaining.push_back(lst.items[j]);
                    }
                    return list(remaining);
                }
                
                // Check truthiness
                bool is_true = true;
                if ((*test_result)->is_nil()) is_true = false;
                if (auto* n = std::get_if<Value::Num>(&(*test_result)->data)) {
                    if (n->val == 0.0) is_true = false;
                }
                
                if (is_true) {
                    return eval(clause->items[1], allow_holes);
                }
            }
            
            return nil();  // No clause matched
        }
        else if (sym->name == "and") {
            // (and expr1 expr2 ...) - Short-circuit logical AND
            if (lst.items.size() == 1) {
                return num(1);  // (and) is true
            }
            
            ValuePtr result = nil();
            for (size_t i = 1; i < lst.items.size(); i++) {
                auto eval_result = eval(lst.items[i], allow_holes);
                if (!eval_result) return eval_result;
                result = *eval_result;
                
                if (result->is_hole()) {
                    if (!allow_holes) {
                        return std::unexpected(Error{ErrorKind::HOLE_ACCESS, "Cannot evaluate and with hole"});
                    }
                    // Return simplified and with remaining expressions
                    ValueList remaining;
                    remaining.push_back(secure::sym("and"));
                    remaining.push_back(result);
                    for (size_t j = i + 1; j < lst.items.size(); j++) {
                        remaining.push_back(lst.items[j]);
                    }
                    return list(remaining);
                }
                
                // Check for false
                if (result->is_nil()) return nil();
                if (auto* n = std::get_if<Value::Num>(&result->data)) {
                    if (n->val == 0.0) return num(0);
                }
            }
            return result;  // Return last value
        }
        else if (sym->name == "or") {
            // (or expr1 expr2 ...) - Short-circuit logical OR
            if (lst.items.size() == 1) {
                return nil();  // (or) is false
            }
            
            for (size_t i = 1; i < lst.items.size(); i++) {
                auto result = eval(lst.items[i], allow_holes);
                if (!result) return result;
                
                if ((*result)->is_hole()) {
                    if (!allow_holes) {
                        return std::unexpected(Error{ErrorKind::HOLE_ACCESS, "Cannot evaluate or with hole"});
                    }
                    // Return simplified or with remaining expressions
                    ValueList remaining;
                    remaining.push_back(secure::sym("or"));
                    remaining.push_back(*result);
                    for (size_t j = i + 1; j < lst.items.size(); j++) {
                        remaining.push_back(lst.items[j]);
                    }
                    return list(remaining);
                }
                
                // Check for true
                bool is_true = true;
                if ((*result)->is_nil()) is_true = false;
                if (auto* n = std::get_if<Value::Num>(&(*result)->data)) {
                    if (n->val == 0.0) is_true = false;
                }
                
                if (is_true) {
                    return *result;  // Return first true value
                }
            }
            return nil();  // All false
        }
        else if (sym->name == "lambda") {
            // (lambda (params...) body)
            if (lst.items.size() != 3) {
                return std::unexpected(Error{ErrorKind::EVAL_ERROR, "Lambda requires params and body"});
            }
            
            auto* params_list = std::get_if<Value::List>(&lst.items[1]->data);
            if (!params_list) {
                return std::unexpected(Error{ErrorKind::TYPE_ERROR, "Lambda params must be a list"});
            }
            
            // Create lambda with current environment captured
            Value::Lambda lam;
            lam.params = params_list->items;
            lam.body = lst.items[2];
            lam.closure = current_frame().bindings;
            
            return std::make_shared<Value>(lam);
        }
        else if (sym->name == "if") {
            // (if cond then else)
            if (lst.items.size() != 4) {
                return std::unexpected(Error{ErrorKind::EVAL_ERROR, "If requires condition, then, and else"});
            }
            
            auto cond = eval(lst.items[1], allow_holes);
            if (!cond) return cond;
            
            // Check if condition has holes
            if ((*cond)->is_hole()) {
                if (!allow_holes) {
                    return std::unexpected(Error{ErrorKind::HOLE_ACCESS, "Cannot evaluate if with hole in condition"});
                }
                // Return simplified if expression with evaluated condition
                return list({secure::sym("if"), *cond, lst.items[2], lst.items[3]});
            }
            
            // Check truthiness (nil and 0 are false)
            bool is_true = true;
            if ((*cond)->is_nil()) is_true = false;
            if (auto* n = std::get_if<Value::Num>(&(*cond)->data)) {
                if (n->val == 0.0) is_true = false;
            }
            
            return eval(is_true ? lst.items[2] : lst.items[3], allow_holes);
        }
        else if (sym->name == "let") {
            // (let ((var val)...) body)
            if (lst.items.size() != 3) {
                return std::unexpected(Error{ErrorKind::EVAL_ERROR, "Let requires bindings and body"});
            }
            
            auto* bindings = std::get_if<Value::List>(&lst.items[1]->data);
            if (!bindings) {
                return std::unexpected(Error{ErrorKind::TYPE_ERROR, "Let bindings must be a list"});
            }
            
            push_frame();
            
            // Evaluate and bind each variable
            for (auto& binding : bindings->items) {
                auto* bind_list = std::get_if<Value::List>(&binding->data);
                if (!bind_list || bind_list->items.size() != 2) {
                    pop_frame();
                    return std::unexpected(Error{ErrorKind::EVAL_ERROR, "Invalid let binding"});
                }
                
                auto* var_sym = std::get_if<Value::Sym>(&bind_list->items[0]->data);
                if (!var_sym) {
                    pop_frame();
                    return std::unexpected(Error{ErrorKind::TYPE_ERROR, "Let variable must be symbol"});
                }
                
                auto val = eval(bind_list->items[1], allow_holes);
                if (!val) {
                    pop_frame();
                    return val;
                }
                
                bind(var_sym->name, *val);
            }
            
            auto result = eval(lst.items[2], allow_holes);
            pop_frame();
            return result;
        }
    }
    
    // Regular function application
    auto fn = eval(first, allow_holes);
    if (!fn) return fn;
    
    // Evaluate arguments
    ValueList args;
    for (size_t i = 1; i < lst.items.size(); i++) {
        auto arg = eval(lst.items[i], allow_holes);
        if (!arg) return arg;
        args.push_back(*arg);
    }
    
    return apply(*fn, args, allow_holes);
}

std::expected<ValuePtr, Error> Evaluator::eval_symbol(const Value::Sym& sym) {
    // Built-in operations
    if (sym.name == "+" || sym.name == "-" || sym.name == "*" || sym.name == "/" ||
        sym.name == "=" || sym.name == "<" || sym.name == ">" ||
        sym.name == "cons" || sym.name == "car" || sym.name == "cdr" || 
        sym.name == "list" || sym.name == "null?" || sym.name == "pair?") {
        return secure::sym(sym.name);
    }
    
    auto val = lookup(sym.name);
    if (!val) {
        return std::unexpected(Error{ErrorKind::EVAL_ERROR, "Undefined symbol: " + sym.name});
    }
    return val;
}

std::expected<ValuePtr, Error> Evaluator::apply(ValuePtr fn, const ValueList& args, bool allow_holes) {
    // Built-in functions
    if (auto* sym = std::get_if<Value::Sym>(&fn->data)) {
        if (sym->name == "+") return builtin_add(args);
        if (sym->name == "-") return builtin_sub(args);
        if (sym->name == "*") return builtin_mul(args);
        if (sym->name == "/") return builtin_div(args);
        
        // List operations
        if (sym->name == "cons") {
            if (args.size() != 2) {
                return std::unexpected(Error{ErrorKind::EVAL_ERROR, "Cons requires exactly 2 arguments"});
            }
            // If second arg is a list, prepend first arg
            if (auto* lst = std::get_if<Value::List>(&args[1]->data)) {
                ValueList new_items;
                new_items.push_back(args[0]);
                new_items.insert(new_items.end(), lst->items.begin(), lst->items.end());
                return list(new_items);
            }
            // Otherwise create a pair
            return list({args[0], args[1]});
        }
        
        if (sym->name == "car") {
            if (args.size() != 1) {
                return std::unexpected(Error{ErrorKind::EVAL_ERROR, "Car requires exactly 1 argument"});
            }
            if (auto* lst = std::get_if<Value::List>(&args[0]->data)) {
                if (lst->items.empty()) {
                    return std::unexpected(Error{ErrorKind::EVAL_ERROR, "Car of empty list"});
                }
                return lst->items[0];
            }
            return std::unexpected(Error{ErrorKind::TYPE_ERROR, "Car requires a list"});
        }
        
        if (sym->name == "cdr") {
            if (args.size() != 1) {
                return std::unexpected(Error{ErrorKind::EVAL_ERROR, "Cdr requires exactly 1 argument"});
            }
            if (auto* lst = std::get_if<Value::List>(&args[0]->data)) {
                if (lst->items.empty()) {
                    return std::unexpected(Error{ErrorKind::EVAL_ERROR, "Cdr of empty list"});
                }
                ValueList rest(lst->items.begin() + 1, lst->items.end());
                return list(rest);
            }
            return std::unexpected(Error{ErrorKind::TYPE_ERROR, "Cdr requires a list"});
        }
        
        if (sym->name == "list") {
            return list(args);
        }
        
        if (sym->name == "null?") {
            if (args.size() != 1) {
                return std::unexpected(Error{ErrorKind::EVAL_ERROR, "Null? requires exactly 1 argument"});
            }
            if (args[0]->is_nil()) {
                return num(1);  // True
            }
            if (auto* lst = std::get_if<Value::List>(&args[0]->data)) {
                return num(lst->items.empty() ? 1 : 0);
            }
            return num(0);  // False for non-nil, non-empty-list
        }
        
        if (sym->name == "pair?") {
            if (args.size() != 1) {
                return std::unexpected(Error{ErrorKind::EVAL_ERROR, "Pair? requires exactly 1 argument"});
            }
            if (auto* lst = std::get_if<Value::List>(&args[0]->data)) {
                return num(!lst->items.empty() ? 1 : 0);
            }
            return num(0);  // False for non-lists
        }
        
        // Comparison operations
        if (sym->name == "=") {
            if (args.size() != 2) {
                return std::unexpected(Error{ErrorKind::EVAL_ERROR, "= requires exactly 2 arguments"});
            }
            if (args[0]->is_hole() || args[1]->is_hole()) {
                if (!allow_holes) {
                    return std::unexpected(Error{ErrorKind::HOLE_ACCESS, "Cannot compare with holes"});
                }
                return list({secure::sym("="), args[0], args[1]});
            }
            auto* n1 = std::get_if<Value::Num>(&args[0]->data);
            auto* n2 = std::get_if<Value::Num>(&args[1]->data);
            if (!n1 || !n2) {
                return std::unexpected(Error{ErrorKind::TYPE_ERROR, "= requires numbers"});
            }
            return num(n1->val == n2->val ? 1 : 0);
        }
        
        if (sym->name == "<") {
            if (args.size() != 2) {
                return std::unexpected(Error{ErrorKind::EVAL_ERROR, "< requires exactly 2 arguments"});
            }
            if (args[0]->is_hole() || args[1]->is_hole()) {
                if (!allow_holes) {
                    return std::unexpected(Error{ErrorKind::HOLE_ACCESS, "Cannot compare with holes"});
                }
                return list({secure::sym("<"), args[0], args[1]});
            }
            auto* n1 = std::get_if<Value::Num>(&args[0]->data);
            auto* n2 = std::get_if<Value::Num>(&args[1]->data);
            if (!n1 || !n2) {
                return std::unexpected(Error{ErrorKind::TYPE_ERROR, "< requires numbers"});
            }
            return num(n1->val < n2->val ? 1 : 0);
        }
        
        if (sym->name == ">") {
            if (args.size() != 2) {
                return std::unexpected(Error{ErrorKind::EVAL_ERROR, "> requires exactly 2 arguments"});
            }
            if (args[0]->is_hole() || args[1]->is_hole()) {
                if (!allow_holes) {
                    return std::unexpected(Error{ErrorKind::HOLE_ACCESS, "Cannot compare with holes"});
                }
                return list({secure::sym(">"), args[0], args[1]});
            }
            auto* n1 = std::get_if<Value::Num>(&args[0]->data);
            auto* n2 = std::get_if<Value::Num>(&args[1]->data);
            if (!n1 || !n2) {
                return std::unexpected(Error{ErrorKind::TYPE_ERROR, "> requires numbers"});
            }
            return num(n1->val > n2->val ? 1 : 0);
        }
        
        return std::unexpected(Error{ErrorKind::EVAL_ERROR, "Unknown function: " + sym->name});
    }
    
    // User-defined lambda
    if (auto* lam = std::get_if<Value::Lambda>(&fn->data)) {
        if (args.size() != lam->params.size()) {
            return std::unexpected(Error{ErrorKind::EVAL_ERROR, "Argument count mismatch"});
        }
        
        push_frame();
        
        // Restore closure environment
        current_frame().bindings = lam->closure;
        
        // Bind parameters
        for (size_t i = 0; i < args.size(); i++) {
            auto* param_sym = std::get_if<Value::Sym>(&lam->params[i]->data);
            if (!param_sym) {
                pop_frame();
                return std::unexpected(Error{ErrorKind::TYPE_ERROR, "Lambda parameter must be symbol"});
            }
            bind(param_sym->name, args[i]);
        }
        
        auto result = eval(lam->body, allow_holes);
        pop_frame();
        return result;
    }
    
    return std::unexpected(Error{ErrorKind::TYPE_ERROR, "Not a function"});
}

std::expected<ValuePtr, Error> Evaluator::builtin_add(const ValueList& args) {
    double sum = 0;
    bool has_hole = false;
    ValueList hole_args;
    
    for (auto& arg : args) {
        if (arg->is_hole()) {
            has_hole = true;
            hole_args.push_back(arg);
        } else if (auto* n = std::get_if<Value::Num>(&arg->data)) {
            sum += n->val;
        } else if (auto* lst = std::get_if<Value::List>(&arg->data)) {
            // Handle partially evaluated expressions in partial eval mode
            has_hole = true;
            hole_args.push_back(arg);
        } else {
            return std::unexpected(Error{ErrorKind::TYPE_ERROR, "Addition requires numbers"});
        }
    }
    
    if (has_hole) {
        // Partial evaluation: return (+ sum ?holes...)
        if (sum != 0) {
            hole_args.insert(hole_args.begin(), num(sum));
        }
        if (hole_args.size() == 1) {
            return hole_args[0];
        }
        hole_args.insert(hole_args.begin(), sym("+"));
        auto result = list(hole_args);
        result->meta.tainted = true;
        return result;
    }
    
    return num(sum);
}

std::expected<ValuePtr, Error> Evaluator::builtin_mul(const ValueList& args) {
    double product = 1;
    bool has_hole = false;
    ValueList hole_args;
    
    for (auto& arg : args) {
        if (arg->is_hole()) {
            has_hole = true;
            hole_args.push_back(arg);
        } else if (auto* n = std::get_if<Value::Num>(&arg->data)) {
            product *= n->val;
        } else if (auto* lst = std::get_if<Value::List>(&arg->data)) {
            // Handle partially evaluated expressions in partial eval mode
            has_hole = true;
            hole_args.push_back(arg);
        } else {
            return std::unexpected(Error{ErrorKind::TYPE_ERROR, "Multiplication requires numbers"});
        }
    }
    
    if (has_hole) {
        // Partial evaluation
        if (product == 0) {
            return num(0);  // 0 * anything = 0
        }
        if (product != 1) {
            hole_args.insert(hole_args.begin(), num(product));
        }
        if (hole_args.size() == 1) {
            return hole_args[0];
        }
        hole_args.insert(hole_args.begin(), sym("*"));
        auto result = list(hole_args);
        result->meta.tainted = true;
        return result;
    }
    
    return num(product);
}

std::expected<ValuePtr, Error> Evaluator::builtin_sub(const ValueList& args) {
    if (args.size() != 2) {
        return std::unexpected(Error{ErrorKind::EVAL_ERROR, "Subtraction requires exactly 2 arguments"});
    }
    
    if (args[0]->is_hole() || args[1]->is_hole()) {
        auto result = list({sym("-"), args[0], args[1]});
        result->meta.tainted = true;
        return result;
    }
    
    auto* n1 = std::get_if<Value::Num>(&args[0]->data);
    auto* n2 = std::get_if<Value::Num>(&args[1]->data);
    
    if (!n1 || !n2) {
        return std::unexpected(Error{ErrorKind::TYPE_ERROR, "Subtraction requires numbers"});
    }
    
    return num(n1->val - n2->val);
}

std::expected<ValuePtr, Error> Evaluator::builtin_div(const ValueList& args) {
    if (args.size() != 2) {
        return std::unexpected(Error{ErrorKind::EVAL_ERROR, "Division requires exactly 2 arguments"});
    }
    
    if (args[0]->is_hole() || args[1]->is_hole()) {
        auto result = list({sym("/"), args[0], args[1]});
        result->meta.tainted = true;
        return result;
    }
    
    auto* n1 = std::get_if<Value::Num>(&args[0]->data);
    auto* n2 = std::get_if<Value::Num>(&args[1]->data);
    
    if (!n1 || !n2) {
        return std::unexpected(Error{ErrorKind::TYPE_ERROR, "Division requires numbers"});
    }
    
    if (n2->val == 0) {
        return std::unexpected(Error{ErrorKind::EVAL_ERROR, "Division by zero"});
    }
    
    return num(n1->val / n2->val);
}

std::expected<ValuePtr, Error> Evaluator::eval_quasiquote(ValuePtr expr, bool allow_holes) {
    // Handle unquote at top level
    if (auto* lst = std::get_if<Value::List>(&expr->data)) {
        if (!lst->items.empty()) {
            if (auto* sym = std::get_if<Value::Sym>(&lst->items[0]->data)) {
                if (sym->name == "unquote") {
                    // (unquote expr) - evaluate the expression
                    if (lst->items.size() != 2) {
                        return std::unexpected(Error{ErrorKind::EVAL_ERROR, "Unquote requires exactly one argument"});
                    }
                    return eval(lst->items[1], allow_holes);
                }
                else if (sym->name == "unquote-splicing") {
                    // unquote-splicing only valid inside a list
                    return std::unexpected(Error{ErrorKind::EVAL_ERROR, "Unquote-splicing not valid at top level"});
                }
            }
        }
        
        // Recursively process list elements
        ValueList result_items;
        for (auto& item : lst->items) {
            // Check for unquote-splicing
            if (auto* item_lst = std::get_if<Value::List>(&item->data)) {
                if (!item_lst->items.empty()) {
                    if (auto* sym = std::get_if<Value::Sym>(&item_lst->items[0]->data)) {
                        if (sym->name == "unquote-splicing") {
                            // (unquote-splicing expr) - evaluate and splice
                            if (item_lst->items.size() != 2) {
                                return std::unexpected(Error{ErrorKind::EVAL_ERROR, "Unquote-splicing requires exactly one argument"});
                            }
                            auto spliced = eval(item_lst->items[1], allow_holes);
                            if (!spliced) return spliced;
                            
                            // Splice the result if it's a list
                            if (auto* splice_lst = std::get_if<Value::List>(&(*spliced)->data)) {
                                result_items.insert(result_items.end(), splice_lst->items.begin(), splice_lst->items.end());
                            } else {
                                // Non-list values just get added
                                result_items.push_back(*spliced);
                            }
                            continue;
                        }
                    }
                }
            }
            
            // Recursively process this item
            auto processed = eval_quasiquote(item, allow_holes);
            if (!processed) return processed;
            result_items.push_back(*processed);
        }
        
        return list(result_items);
    }
    
    // Non-list expressions are returned as-is (like quote)
    return expr;
}

ValuePtr Evaluator::lookup(const std::string& name) {
    // Search from innermost to outermost scope
    for (auto it = call_stack.rbegin(); it != call_stack.rend(); ++it) {
        auto found = it->bindings.find(name);
        if (found != it->bindings.end()) {
            return found->second;
        }
    }
    
    // Check globals
    auto found = globals.find(name);
    if (found != globals.end()) {
        return found->second;
    }
    
    return nullptr;
}

void Evaluator::bind(const std::string& name, ValuePtr val) {
    current_frame().bindings[name] = val;
}

std::expected<ValuePtr, Error> Evaluator::partial_eval(ValuePtr expr) {
    // Save current frame to preserve state
    size_t frame_count = call_stack.size();
    
    auto result = eval(expr, true);  // Allow holes
    
    // Restore frame count
    while (call_stack.size() > frame_count) {
        pop_frame();
    }
    
    return result;
}

std::expected<ValuePtr, Error> Evaluator::fill_holes(ValuePtr expr, 
                                                      const std::unordered_map<std::string, ValuePtr>& values) {
    if (!expr) return nil();
    
    return std::visit([this, &values](auto&& arg) -> std::expected<ValuePtr, Error> {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, Value::Hole>) {
            auto it = values.find(arg.id);
            if (it != values.end()) {
                if (arg.sealed) {
                    return std::unexpected(Error{ErrorKind::SECURITY_VIOLATION, 
                        "Cannot fill sealed hole: " + arg.id});
                }
                return it->second;
            }
            return hole(arg.id, arg.sealed);
        } else if constexpr (std::is_same_v<T, Value::List>) {
            ValueList filled;
            for (auto& item : arg.items) {
                auto result = fill_holes(item, values);
                if (!result) return result;
                filled.push_back(*result);
            }
            return list(filled);
        } else if constexpr (std::is_same_v<T, Value::Lambda>) {
            // Fill holes in lambda body
            auto filled_body = fill_holes(arg.body, values);
            if (!filled_body) return filled_body;
            
            Value::Lambda new_lam = arg;
            new_lam.body = *filled_body;
            return std::make_shared<Value>(new_lam);
        } else {
            return std::make_shared<Value>(arg);
        }
    }, expr->data);
}

std::vector<std::string> Evaluator::find_holes(ValuePtr expr) {
    std::vector<std::string> holes;
    
    if (!expr) return holes;
    
    std::visit([&holes, this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, Value::Hole>) {
            holes.push_back(arg.id);
        } else if constexpr (std::is_same_v<T, Value::List>) {
            for (auto& item : arg.items) {
                auto sub_holes = find_holes(item);
                holes.insert(holes.end(), sub_holes.begin(), sub_holes.end());
            }
        } else if constexpr (std::is_same_v<T, Value::Lambda>) {
            auto body_holes = find_holes(arg.body);
            holes.insert(holes.end(), body_holes.begin(), body_holes.end());
        }
    }, expr->data);
    
    // Remove duplicates
    std::sort(holes.begin(), holes.end());
    holes.erase(std::unique(holes.begin(), holes.end()), holes.end());
    
    return holes;
}

// Pretty printer
std::string to_string(const ValuePtr& val) {
    if (!val) return "nil";
    
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, Value::Nil>) {
            return "nil";
        } else if constexpr (std::is_same_v<T, Value::Hole>) {
            return "?" + arg.id + (arg.sealed ? "[sealed]" : "");
        } else if constexpr (std::is_same_v<T, Value::Num>) {
            return std::to_string(arg.val);
        } else if constexpr (std::is_same_v<T, Value::Str>) {
            return "\"" + arg.val + "\"";
        } else if constexpr (std::is_same_v<T, Value::Sym>) {
            return arg.name;
        } else if constexpr (std::is_same_v<T, Value::List>) {
            std::string result = "(";
            for (size_t i = 0; i < arg.items.size(); i++) {
                if (i > 0) result += " ";
                result += to_string(arg.items[i]);
            }
            result += ")";
            return result;
        } else if constexpr (std::is_same_v<T, Value::Lambda>) {
            std::string params = "(";
            for (size_t i = 0; i < arg.params.size(); i++) {
                if (i > 0) params += " ";
                params += to_string(arg.params[i]);
            }
            params += ")";
            return "(lambda " + params + " " + to_string(arg.body) + ")";
        }
    }, val->data);
}

} // namespace secure