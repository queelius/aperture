/**
 * @file aperture.hpp
 * @brief Aperture Language - Security-focused programming language with apertures
 * @author Alexander Towell
 * @version 1.0.0
 * 
 * @mainpage Aperture Language Documentation
 * 
 * @section intro Introduction
 * Aperture is a security-focused programming language that enables secure distributed
 * computation through the concept of "apertures" - partial program specifications with
 * holes that can be filled later.
 * 
 * @section features Key Features
 * - **Partial Evaluation**: Expressions with holes reduce as much as possible
 * - **Privacy Preservation**: Sensitive data never leaves trusted environment  
 * - **Algebraic Simplification**: Mathematical expressions automatically simplify
 * - **Fluent C++ API**: Natural syntax for building expressions in C++
 * 
 * @section usage Basic Usage
 * @code{.cpp}
 * using namespace secure;
 * 
 * // Parse and evaluate an expression
 * Parser parser("(+ 10 (* ?x 2))");
 * auto expr = parser.parse();
 * 
 * Evaluator eval;
 * auto partial = eval.partial_eval(*expr);  // Simplify with holes
 * 
 * // Fill holes and evaluate
 * std::unordered_map<std::string, ValuePtr> values{{"x", num(5)}};
 * auto filled = eval.fill_holes(*partial, values);
 * auto result = eval.eval(*filled);
 * @endcode
 */

#pragma once
#include <variant>
#include <memory>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>
#include <optional>
#include <expected>

/**
 * @namespace secure
 * @brief Core namespace for the Aperture secure language implementation
 */
namespace secure {

// Forward declarations
struct Value;
using ValuePtr = std::shared_ptr<Value>;
using ValueList = std::vector<ValuePtr>;

/**
 * @enum ErrorKind
 * @brief Types of errors that can occur during parsing and evaluation
 */
enum class ErrorKind {
    PARSE_ERROR,        ///< Syntax error during parsing
    EVAL_ERROR,         ///< Runtime error during evaluation
    TYPE_ERROR,         ///< Type mismatch error
    HOLE_ACCESS,        ///< Attempted to evaluate expression with holes
    SECURITY_VIOLATION  ///< Security constraint violated
};

/**
 * @struct Error
 * @brief Error information with kind, message, and position
 */
struct Error {
    ErrorKind kind;        ///< Type of error
    std::string message;   ///< Human-readable error message
    size_t position = 0;   ///< Position in source where error occurred
};

/**
 * @struct Value
 * @brief Core value type representing all possible Aperture values
 * 
 * Uses std::variant for type-safe discriminated union of value types.
 * Includes metadata for security tracking (taint analysis, complexity).
 */
struct Value {
    /**
     * @struct Nil
     * @brief Represents empty/null value
     */
    struct Nil {};
    
    /**
     * @struct Hole
     * @brief Represents a placeholder for private/unknown data
     * 
     * Holes are the key innovation that enables secure distributed computation.
     * They act as placeholders that preserve privacy while allowing optimization.
     */
    struct Hole { 
        std::string id;                      ///< Unique identifier for the hole
        std::optional<std::string> constraint; ///< Optional type/range constraint
        bool sealed = false;                  ///< If true, hole cannot be modified
    };
    
    /**
     * @struct Num
     * @brief Numeric value (double precision)
     */
    struct Num { 
        double val; ///< Numeric value
    };
    
    /**
     * @struct Sym
     * @brief Symbol/identifier value
     */
    struct Sym { 
        std::string name; ///< Symbol name
    };
    
    /**
     * @struct Str
     * @brief String value
     */
    struct Str { 
        std::string val; ///< String content
    };
    
    /**
     * @struct List
     * @brief List of values (S-expression)
     */
    struct List { 
        ValueList items; ///< List elements
    };
    
    /**
     * @struct Lambda
     * @brief Function closure with captured environment
     */
    struct Lambda {
        ValueList params;  ///< Parameter list
        ValuePtr body;     ///< Function body expression
        std::unordered_map<std::string, ValuePtr> closure; ///< Captured bindings
    };
    
    /// Variant holding the actual value
    using Data = std::variant<Nil, Hole, Num, Sym, Str, List, Lambda>;
    Data data;
    
    /**
     * @struct Meta
     * @brief Security and performance metadata
     */
    struct Meta {
        bool tainted = false;              ///< Has this value touched a hole?
        size_t complexity = 1;             ///< Complexity measure for DoS prevention
        std::vector<std::string> holes_used; ///< Track hole dependencies
    } meta;
    
    /// Default constructor creates nil value
    Value() : data(Nil{}) {}
    
    /// Construct from specific type
    Value(Data d) : data(std::move(d)) {}
    
    /// @name Type checking predicates
    /// @{
    bool is_nil() const { return std::holds_alternative<Nil>(data); }
    bool is_hole() const { return std::holds_alternative<Hole>(data); }
    bool is_num() const { return std::holds_alternative<Num>(data); }
    bool is_sym() const { return std::holds_alternative<Sym>(data); }
    bool is_str() const { return std::holds_alternative<Str>(data); }
    bool is_list() const { return std::holds_alternative<List>(data); }
    bool is_lambda() const { return std::holds_alternative<Lambda>(data); }
    /// @}
    
    /// Safe accessor for variant data
    template<typename T>
    std::optional<T*> get_if() { return std::get_if<T>(&data); }
    
    template<typename T>
    const std::optional<const T*> get_if() const { return std::get_if<T>(&data); }
};

/// @name Smart constructors
/// @{
/**
 * @brief Create a nil value
 * @return Shared pointer to nil value
 */
inline ValuePtr nil() { return std::make_shared<Value>(Value::Nil{}); }

/**
 * @brief Create a hole (placeholder for private data)
 * @param id Unique identifier for the hole
 * @param sealed If true, hole cannot be modified after creation
 * @return Shared pointer to hole value
 */
inline ValuePtr hole(std::string id, bool sealed = false) { 
    return std::make_shared<Value>(Value::Hole{std::move(id), std::nullopt, sealed}); 
}

/**
 * @brief Create a numeric value
 * @param n Numeric value
 * @return Shared pointer to number value
 */
inline ValuePtr num(double n) { return std::make_shared<Value>(Value::Num{n}); }

/**
 * @brief Create a symbol value
 * @param s Symbol name
 * @return Shared pointer to symbol value
 */
inline ValuePtr sym(std::string s) { return std::make_shared<Value>(Value::Sym{std::move(s)}); }

/**
 * @brief Create a string value
 * @param s String content
 * @return Shared pointer to string value
 */
inline ValuePtr str(std::string s) { return std::make_shared<Value>(Value::Str{std::move(s)}); }

/**
 * @brief Create a list value
 * @param items List elements
 * @return Shared pointer to list value
 */
inline ValuePtr list(ValueList items) { return std::make_shared<Value>(Value::List{std::move(items)}); }
/// @}

/**
 * @class Parser
 * @brief Recursive descent parser for S-expressions with hole syntax
 * 
 * Parses Aperture language syntax including:
 * - S-expressions: (operator arg1 arg2 ...)
 * - Numbers: 42, 3.14, 1e-10
 * - Strings: "hello world"
 * - Symbols: x, +, lambda
 * - Holes: ?variable or ?{variable:constraint}
 * 
 * @code{.cpp}
 * Parser parser("(+ 10 ?x)");
 * auto expr = parser.parse();
 * if (expr) {
 *     // Use *expr
 * } else {
 *     // Handle expr.error()
 * }
 * @endcode
 */
class Parser {
    std::string_view input;  ///< Input source code
    size_t pos = 0;          ///< Current position in input
    
public:
    /**
     * @brief Construct parser from source code
     * @param in Source code to parse
     */
    explicit Parser(std::string_view in) : input(in) {}
    
    /**
     * @brief Parse input and return expression or error
     * @return Expected containing parsed expression or error
     */
    std::expected<ValuePtr, Error> parse();
    
private:
    void skip_whitespace();
    std::expected<ValuePtr, Error> parse_value();
    std::expected<ValuePtr, Error> parse_list();
    std::expected<ValuePtr, Error> parse_atom();
    std::expected<double, Error> parse_number();
    std::expected<std::string, Error> parse_string();
    std::expected<std::string, Error> parse_symbol();
    
    bool peek(char c) const { return pos < input.size() && input[pos] == c; }
    bool consume(char c);
    char current() const { return pos < input.size() ? input[pos] : '\0'; }
    bool at_end() const { return pos >= input.size(); }
};

/**
 * @class Evaluator
 * @brief Stack-based expression evaluator with partial evaluation support
 * 
 * Core evaluation engine that supports:
 * - Normal evaluation: reduce expressions to values
 * - Partial evaluation: simplify expressions with holes
 * - Hole filling: substitute values for holes
 * - Security limits: prevent DoS attacks
 * 
 * @code{.cpp}
 * Evaluator eval;
 * 
 * // Normal evaluation
 * auto result = eval.eval(expr);
 * 
 * // Partial evaluation with holes
 * auto partial = eval.partial_eval(expr);
 * 
 * // Fill holes and evaluate
 * std::unordered_map<std::string, ValuePtr> values{{"x", num(10)}};
 * auto filled = eval.fill_holes(partial, values);
 * auto final = eval.eval(filled);
 * @endcode
 */
class Evaluator {
public:
    /**
     * @struct Frame
     * @brief Call frame for function evaluation
     */
    struct Frame {
        std::unordered_map<std::string, ValuePtr> bindings; ///< Local bindings
        ValuePtr return_value = nil();                      ///< Return value
    };
    
private:
    std::vector<Frame> call_stack;                          ///< Function call stack
    std::unordered_map<std::string, ValuePtr> globals;      ///< Global bindings
    size_t max_depth = 1000;                                ///< Max recursion depth
    size_t max_complexity = 1000000;                        ///< Max complexity score
    
public:
    /// Construct evaluator with built-in functions
    Evaluator();
    
    /**
     * @brief Evaluate expression to a value
     * @param expr Expression to evaluate
     * @param allow_holes If true, holes are allowed in result
     * @return Evaluated value or error
     */
    std::expected<ValuePtr, Error> eval(ValuePtr expr, bool allow_holes = false);
    
    /**
     * @brief Partially evaluate expression, simplifying while preserving holes
     * @param expr Expression to partially evaluate
     * @return Simplified expression or error
     * 
     * Performs algebraic simplification while preserving holes:
     * - (+ 10 ?x 5) becomes (+ 15 ?x)
     * - (* 0 ?x) becomes 0
     * - (if ?cond A B) remains unchanged
     */
    std::expected<ValuePtr, Error> partial_eval(ValuePtr expr);
    
    /**
     * @brief Fill holes in expression with provided values
     * @param expr Expression containing holes
     * @param values Mapping from hole names to values
     * @return Expression with holes filled or error
     */
    std::expected<ValuePtr, Error> fill_holes(ValuePtr expr, 
                                              const std::unordered_map<std::string, ValuePtr>& values);
    
    /**
     * @brief Find all hole names in an expression
     * @param expr Expression to search
     * @return Vector of unique hole names
     */
    std::vector<std::string> find_holes(ValuePtr expr);
    
    /**
     * @brief Make holes in expression immutable
     * @param expr Expression containing holes
     * @return Expression with sealed holes
     */
    ValuePtr seal_holes(ValuePtr expr);
    
    /**
     * @brief Check if expression is safe to evaluate
     * @param expr Expression to check
     * @return True if expression passes security checks
     */
    bool is_safe(ValuePtr expr);
    
private:
    std::expected<ValuePtr, Error> eval_list(const Value::List& lst, bool allow_holes);
    std::expected<ValuePtr, Error> eval_symbol(const Value::Sym& sym);
    std::expected<ValuePtr, Error> apply(ValuePtr fn, const ValueList& args, bool allow_holes);
    std::expected<ValuePtr, Error> eval_quasiquote(ValuePtr expr, bool allow_holes);
    
    // Built-in operations
    std::expected<ValuePtr, Error> builtin_add(const ValueList& args);
    std::expected<ValuePtr, Error> builtin_mul(const ValueList& args);
    std::expected<ValuePtr, Error> builtin_sub(const ValueList& args);
    std::expected<ValuePtr, Error> builtin_div(const ValueList& args);
    std::expected<ValuePtr, Error> builtin_if(const ValueList& args, bool allow_holes);
    
    void push_frame() { call_stack.emplace_back(); }
    void pop_frame() { if (!call_stack.empty()) call_stack.pop_back(); }
    Frame& current_frame() { return call_stack.back(); }
    
    ValuePtr lookup(const std::string& name);
    void bind(const std::string& name, ValuePtr val);
};

/**
 * @brief Convert value to string representation
 * @param val Value to convert
 * @return String representation
 * 
 * Examples:
 * - nil -> "nil"
 * - 42 -> "42.000000"
 * - ?x -> "?x"
 * - (+ 1 2) -> "(+ 1.000000 2.000000)"
 */
std::string to_string(const ValuePtr& val);

/**
 * @struct SecurityContext
 * @brief Security constraints for expression evaluation
 * 
 * Defines security policies to prevent attacks:
 * - Limit execution time
 * - Limit memory usage
 * - Restrict certain operations
 * - Control which holes are allowed
 */
struct SecurityContext {
    std::vector<std::string> allowed_holes;     ///< Whitelist of allowed hole names
    std::vector<std::string> forbidden_ops;     ///< Blacklist of forbidden operations
    size_t max_execution_time_ms = 1000;        ///< Maximum execution time
    size_t max_memory_bytes = 100'000'000;      ///< Maximum memory usage (100MB)
    
    /**
     * @brief Validate expression against security constraints
     * @param expr Expression to validate
     * @return True if expression passes all security checks
     */
    bool validate(const ValuePtr& expr) const;
};

} // namespace secure