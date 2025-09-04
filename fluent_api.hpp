#pragma once
#include "aperture.hpp"
#include <functional>
#include <iostream>
#include <type_traits>

namespace aperture {

// Forward declarations
class Expr;
class Computation;
class SecureComputation;

// Type-safe hole wrapper
template<typename T = double>
class Hole {
    std::string name;
    T value;
    bool filled = false;
    
public:
    explicit Hole(std::string n) : name(std::move(n)) {}
    
    Hole& operator=(T val) {
        value = val;
        filled = true;
        return *this;
    }
    
    const std::string& get_name() const { return name; }
    T get_value() const { return value; }
    bool is_filled() const { return filled; }
    
    operator Expr() const;
};

// Expression builder class with operator overloading
class Expr {
    secure::ValuePtr val;
    
    friend class Computation;
    friend class SecureComputation;
    
public:
    // Constructors
    Expr() : val(secure::nil()) {}
    Expr(double n) : val(secure::num(n)) {}
    Expr(int n) : val(secure::num(static_cast<double>(n))) {}
    Expr(const std::string& s) : val(secure::sym(s)) {}
    Expr(const char* s) : val(secure::sym(s)) {}
    Expr(secure::ValuePtr v) : val(std::move(v)) {}
    
    // Create a hole
    static Expr hole(const std::string& name) {
        return Expr(secure::hole(name));
    }
    
    // Arithmetic operators
    Expr operator+(const Expr& other) const {
        return Expr(secure::list({secure::sym("+"), val, other.val}));
    }
    
    Expr operator-(const Expr& other) const {
        return Expr(secure::list({secure::sym("-"), val, other.val}));
    }
    
    Expr operator*(const Expr& other) const {
        return Expr(secure::list({secure::sym("*"), val, other.val}));
    }
    
    Expr operator/(const Expr& other) const {
        return Expr(secure::list({secure::sym("/"), val, other.val}));
    }
    
    // Unary minus
    Expr operator-() const {
        return Expr(secure::list({secure::sym("-"), secure::num(0), val}));
    }
    
    // Comparison operators (return expressions, not bools)
    Expr operator>(const Expr& other) const {
        return Expr(secure::list({secure::sym(">"), val, other.val}));
    }
    
    Expr operator<(const Expr& other) const {
        return Expr(secure::list({secure::sym("<"), val, other.val}));
    }
    
    Expr operator==(const Expr& other) const {
        return Expr(secure::list({secure::sym("="), val, other.val}));
    }
    
    // Compound assignment operators
    Expr& operator+=(const Expr& other) {
        val = secure::list({secure::sym("+"), val, other.val});
        return *this;
    }
    
    Expr& operator*=(const Expr& other) {
        val = secure::list({secure::sym("*"), val, other.val});
        return *this;
    }
    
    // Function call operator for lambda application
    template<typename... Args>
    Expr operator()(Args&&... args) const {
        secure::ValueList argList = {val};
        (argList.push_back(Expr(std::forward<Args>(args)).val), ...);
        return Expr(secure::list(argList));
    }
    
    // String representation
    std::string to_string() const {
        return secure::to_string(val);
    }
    
    // Get underlying value
    secure::ValuePtr get_value() const { return val; }
    
    // Check if expression contains holes
    bool has_holes() const {
        secure::Evaluator eval;
        auto holes = eval.find_holes(val);
        return !holes.empty();
    }
    
    // Get list of hole names
    std::vector<std::string> get_holes() const {
        secure::Evaluator eval;
        return eval.find_holes(val);
    }
};

// Hole conversion to Expr
template<typename T>
Hole<T>::operator Expr() const {
    return Expr::hole(name);
}

// Global functions for common operations
namespace ops {
    // Conditional
    inline Expr if_then_else(const Expr& cond, const Expr& then_branch, const Expr& else_branch) {
        return Expr(secure::list({
            secure::sym("if"),
            cond.get_value(),
            then_branch.get_value(),
            else_branch.get_value()
        }));
    }
    
    // Let binding - single binding
    inline Expr let(const std::string& var, const Expr& value, const Expr& body) {
        return Expr(secure::list({
            secure::sym("let"),
            secure::list({
                secure::list({secure::sym(var), value.get_value()})
            }),
            body.get_value()
        }));
    }
    
    // Let binding - multiple bindings
    inline Expr let(const std::vector<std::pair<std::string, Expr>>& bindings, const Expr& body) {
        secure::ValueList bindingList;
        for (const auto& [var, val] : bindings) {
            bindingList.push_back(secure::list({secure::sym(var), val.get_value()}));
        }
        
        return Expr(secure::list({
            secure::sym("let"),
            secure::list(bindingList),
            body.get_value()
        }));
    }
    
    // Lambda creation
    template<typename F>
    class LambdaBuilder {
        std::vector<std::string> params;
        
    public:
        LambdaBuilder(std::initializer_list<std::string> p) : params(p) {}
        
        Expr operator[](F body_fn) const {
            // Create parameter expressions
            std::vector<Expr> param_exprs;
            for (const auto& p : params) {
                param_exprs.push_back(Expr(p));
            }
            
            // Call the body function with parameter expressions
            Expr body = std::apply(body_fn, 
                std::make_tuple(param_exprs[0]));  // Simplified for single param
            
            // Build lambda expression
            secure::ValueList paramList;
            for (const auto& p : params) {
                paramList.push_back(secure::sym(p));
            }
            
            return Expr(secure::list({
                secure::sym("lambda"),
                secure::list(paramList),
                body.get_value()
            }));
        }
    };
    
    // Simplified lambda for single parameter
    inline Expr lambda(const std::string& param, std::function<Expr(Expr)> body) {
        Expr param_expr(param);
        Expr body_expr = body(param_expr);
        
        return Expr(secure::list({
            secure::sym("lambda"),
            secure::list({secure::sym(param)}),
            body_expr.get_value()
        }));
    }
    
    // Multi-parameter lambda
    inline Expr lambda(const std::vector<std::string>& params, 
                      std::function<Expr(std::vector<Expr>)> body) {
        std::vector<Expr> param_exprs;
        secure::ValueList paramList;
        
        for (const auto& p : params) {
            param_exprs.push_back(Expr(p));
            paramList.push_back(secure::sym(p));
        }
        
        Expr body_expr = body(param_exprs);
        
        return Expr(secure::list({
            secure::sym("lambda"),
            secure::list(paramList),
            body_expr.get_value()
        }));
    }
}

// Computation builder for evaluation
class Computation {
protected:
    secure::Evaluator evaluator;
    Expr expression;
    
public:
    Computation() = default;
    explicit Computation(const Expr& expr) : expression(expr) {}
    
    // Set the expression
    Computation& with(const Expr& expr) {
        expression = expr;
        return *this;
    }
    
    // Evaluate the expression
    double eval() {
        auto result = evaluator.eval(expression.get_value());
        if (!result) {
            throw std::runtime_error("Evaluation failed: " + result.error().message);
        }
        
        if (auto* n = std::get_if<secure::Value::Num>(&(*result)->data)) {
            return n->val;
        }
        
        throw std::runtime_error("Result is not a number");
    }
    
    // Evaluate to expression
    Expr eval_expr() {
        auto result = evaluator.eval(expression.get_value());
        if (!result) {
            throw std::runtime_error("Evaluation failed: " + result.error().message);
        }
        return Expr(*result);
    }
    
    // Partial evaluation
    Expr partial_eval() {
        auto result = evaluator.partial_eval(expression.get_value());
        if (!result) {
            throw std::runtime_error("Partial evaluation failed: " + result.error().message);
        }
        return Expr(*result);
    }
    
    // Fill holes
    template<typename... Args>
    Computation& fill(const std::string& name, double value, Args... args) {
        std::unordered_map<std::string, secure::ValuePtr> values;
        values[name] = secure::num(value);
        
        // Recursively fill remaining holes
        if constexpr (sizeof...(args) > 0) {
            fill_impl(values, args...);
        }
        
        auto filled = evaluator.fill_holes(expression.get_value(), values);
        if (!filled) {
            throw std::runtime_error("Failed to fill holes");
        }
        
        expression = Expr(*filled);
        return *this;
    }
    
    // Fill with map
    Computation& fill(const std::unordered_map<std::string, double>& values) {
        std::unordered_map<std::string, secure::ValuePtr> val_map;
        for (const auto& [name, val] : values) {
            val_map[name] = secure::num(val);
        }
        
        auto filled = evaluator.fill_holes(expression.get_value(), val_map);
        if (!filled) {
            throw std::runtime_error("Failed to fill holes");
        }
        
        expression = Expr(*filled);
        return *this;
    }
    
    // Get holes
    std::vector<std::string> holes() const {
        return expression.get_holes();
    }
    
    // String representation
    std::string to_string() const {
        return expression.to_string();
    }
    
private:
    template<typename... Args>
    void fill_impl(std::unordered_map<std::string, secure::ValuePtr>& values,
                   const std::string& name, double value, Args... args) {
        values[name] = secure::num(value);
        if constexpr (sizeof...(args) > 0) {
            fill_impl(values, args...);
        }
    }
};

// Secure computation for untrusted evaluation
class SecureComputation : public Computation {
    bool allow_partial = true;
    
public:
    using Computation::Computation;
    
    // Configure security settings
    SecureComputation& allow_partial_eval(bool allow) {
        allow_partial = allow;
        return *this;
    }
    
    // Send to untrusted server (simulated)
    Expr send_to_server() {
        if (!allow_partial) {
            throw std::runtime_error("Expression contains holes but partial eval not allowed");
        }
        
        std::cout << "[CLIENT->SERVER] Sending: " << expression.to_string() << "\n";
        
        // Server performs partial evaluation
        auto optimized = partial_eval();
        
        std::cout << "[SERVER->CLIENT] Optimized: " << optimized.to_string() << "\n";
        
        return optimized;
    }
    
    // Verify expression hasn't been tampered with
    bool verify(const Expr& received) {
        auto holes_original = expression.get_holes();
        auto holes_received = received.get_holes();
        
        // Check all original holes are preserved
        for (const auto& hole : holes_original) {
            if (std::find(holes_received.begin(), holes_received.end(), hole) 
                == holes_received.end()) {
                return false;
            }
        }
        
        return true;
    }
    
    // Complete secure computation workflow
    double secure_compute(const std::unordered_map<std::string, double>& private_data) {
        // Send to server for optimization
        auto optimized = send_to_server();
        
        // Verify integrity
        if (!verify(optimized)) {
            throw std::runtime_error("Security verification failed!");
        }
        
        // Fill private data locally
        expression = optimized;
        fill(private_data);
        
        // Compute final result
        return eval();
    }
};

// Convenient namespace for building expressions
namespace build {
    // Create a hole
    inline Expr hole(const std::string& name) {
        return Expr::hole(name);
    }
    
    // Create a number
    inline Expr num(double n) {
        return Expr(n);
    }
    
    // Create a symbol
    inline Expr sym(const std::string& s) {
        return Expr(s);
    }
    
    // Shorthand for common holes
    inline Expr x() { return hole("x"); }
    inline Expr y() { return hole("y"); }
    inline Expr z() { return hole("z"); }
}

// Stream operator
inline std::ostream& operator<<(std::ostream& os, const Expr& expr) {
    return os << expr.to_string();
}

} // namespace aperture