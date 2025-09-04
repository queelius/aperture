#include "fluent_api.hpp"
#include <iostream>
#include <iomanip>

using namespace aperture;
using namespace aperture::ops;
using namespace aperture::build;

void demo_basic_arithmetic() {
    std::cout << "\n=== Basic Arithmetic with Fluent API ===\n";
    
    // Natural arithmetic syntax
    Expr result = (num(10) + num(20)) * num(3) - num(5);
    
    std::cout << "Expression: " << result << "\n";
    
    // Evaluate
    Computation comp(result);
    std::cout << "Result: " << comp.eval() << "\n";
}

void demo_holes_and_partial_eval() {
    std::cout << "\n=== Holes and Partial Evaluation ===\n";
    
    // Create expression with holes using natural syntax
    auto expr = (hole("x") + 10) * (hole("y") - 5);
    
    std::cout << "Original: " << expr << "\n";
    
    // Partial evaluation
    Computation comp(expr);
    auto partial = comp.partial_eval();
    std::cout << "Partial: " << partial << "\n";
    
    // Fill holes and evaluate
    double result = comp.fill("x", 3, "y", 8).eval();
    std::cout << "With x=3, y=8: " << result << "\n";
}

void demo_let_bindings() {
    std::cout << "\n=== Let Bindings ===\n";
    
    // Single binding
    auto expr1 = let("x", 10, 
                    Expr("x") * 2 + 5);
    
    std::cout << "Single binding: " << expr1 << "\n";
    std::cout << "Result: " << Computation(expr1).eval() << "\n";
    
    // Multiple bindings
    auto expr2 = let({
        {"a", 5},
        {"b", 10},
        {"c", 15}
    }, Expr("a") + Expr("b") * Expr("c"));
    
    std::cout << "Multiple bindings: " << expr2 << "\n";
    std::cout << "Result: " << Computation(expr2).eval() << "\n";
}

void demo_lambda_functions() {
    std::cout << "\n=== Lambda Functions ===\n";
    
    // Simple lambda: λx. x * 2 + 1
    auto double_plus_one = lambda("x", [](Expr x) {
        return x * 2 + 1;
    });
    
    std::cout << "Lambda: " << double_plus_one << "\n";
    
    // Apply lambda
    auto applied = double_plus_one(10);
    std::cout << "Applied to 10: " << applied << "\n";
    std::cout << "Result: " << Computation(applied).eval() << "\n";
    
    // Multi-parameter lambda: λ(x,y). x² + y²
    auto pythagorean = lambda({"x", "y"}, [](std::vector<Expr> params) {
        return params[0] * params[0] + params[1] * params[1];
    });
    
    std::cout << "Two-param lambda: " << pythagorean << "\n";
}

void demo_conditionals() {
    std::cout << "\n=== Conditionals ===\n";
    
    // if x > 10 then x * 2 else x + 5
    auto expr = if_then_else(
        hole("x") > 10,
        hole("x") * 2,
        hole("x") + 5
    );
    
    std::cout << "Conditional: " << expr << "\n";
    
    // Test with x = 15
    Computation comp1(expr);
    comp1.fill("x", 15);
    std::cout << "With x=15: " << comp1.eval() << "\n";
    
    // Test with x = 5
    Computation comp2(expr);
    comp2.fill("x", 5);
    std::cout << "With x=5: " << comp2.eval() << "\n";
}

void demo_secure_computation() {
    std::cout << "\n=== Secure Computation Workflow ===\n";
    
    // Financial calculation with private data
    auto portfolio_risk = let({
        {"btc_vol", 0.8},
        {"eth_vol", 0.9},
        {"risk_free", 0.02}
    }, 
    (hole("btc_position") * Expr("btc_vol")) * (hole("btc_position") * Expr("btc_vol")) +
    (hole("eth_position") * Expr("eth_vol")) * (hole("eth_position") * Expr("eth_vol")) +
    hole("cash") * Expr("risk_free"));
    
    std::cout << "Portfolio risk formula: " << portfolio_risk << "\n";
    
    // Secure computation
    SecureComputation secure(portfolio_risk);
    
    // Private data (never sent to server)
    std::unordered_map<std::string, double> private_data = {
        {"btc_position", 2.5},
        {"eth_position", 15.0},
        {"cash", 50000}
    };
    
    // Execute secure computation
    double risk = secure.secure_compute(private_data);
    std::cout << "Computed risk: " << risk << "\n";
}

void demo_complex_expressions() {
    std::cout << "\n=== Complex Expression Building ===\n";
    
    // Build quadratic formula: ax² + bx + c
    auto quadratic = [](Expr a, Expr b, Expr c, Expr x) {
        return a * x * x + b * x + c;
    };
    
    // Create with some holes
    auto expr = quadratic(num(2), hole("b"), num(3), hole("x"));
    
    std::cout << "Quadratic: " << expr << "\n";
    
    // Partial evaluation
    Computation comp(expr);
    auto partial = comp.partial_eval();
    std::cout << "Partial: " << partial << "\n";
    
    // Fill and evaluate
    double result = comp.fill("b", 5, "x", 4).eval();
    std::cout << "f(4) with b=5: " << result << "\n";
}

void demo_chaining_operations() {
    std::cout << "\n=== Chaining Operations ===\n";
    
    // Build complex expression with chaining
    Expr x = hole("x");
    Expr y = hole("y");
    
    // Chain multiple operations
    Expr complex = ((x + 5) * (y - 3)) / (x * x + y * y);
    
    std::cout << "Complex expression: " << complex << "\n";
    
    // Create computation and chain operations
    auto result = Computation(complex)
        .fill("x", 3)
        .fill("y", 4)
        .eval();
    
    std::cout << "Result with x=3, y=4: " << result << "\n";
}

void demo_builder_pattern() {
    std::cout << "\n=== Builder Pattern ===\n";
    
    // Use builder namespace for cleaner syntax
    auto expr = (x() + y()) * (x() - y());  // x² - y²
    
    std::cout << "Expression: " << expr << "\n";
    
    // Evaluate with different values
    Computation comp(expr);
    
    std::cout << "Holes needed: ";
    for (const auto& h : comp.holes()) {
        std::cout << h << " ";
    }
    std::cout << "\n";
    
    // Fill and evaluate
    double result = comp.fill({{"x", 10}, {"y", 6}}).eval();
    std::cout << "10² - 6² = " << result << "\n";
}

void demo_medical_dosage() {
    std::cout << "\n=== Medical Dosage Calculation ===\n";
    
    // Complex medical formula
    auto dosage = let("base_dose", 100,
        if_then_else(
            hole("age") > 65,
            Expr("base_dose") * 0.8 * (hole("weight") / num(70)),
            Expr("base_dose") * 1.0 * (hole("weight") / num(70))
        ) * (num(1) + hole("severity") * 0.5)
    );
    
    SecureComputation secure(dosage);
    
    // Hospital sends formula to cloud for optimization
    auto optimized = secure.send_to_server();
    
    // Patient data (private)
    std::unordered_map<std::string, double> patient = {
        {"age", 45},
        {"weight", 80},
        {"severity", 0.7}
    };
    
    // Compute locally with private data
    secure.with(optimized).fill(patient);
    double dose = secure.eval();
    
    std::cout << "Recommended dosage: " << std::fixed << std::setprecision(2) 
              << dose << " mg\n";
}

void demo_symbolic_math() {
    std::cout << "\n=== Symbolic Mathematics ===\n";
    
    // Create symbolic expression: (x + y)²
    auto symbolic = lambda({"x", "y"}, [](std::vector<Expr> p) {
        auto sum = p[0] + p[1];
        return sum * sum;
    });
    
    std::cout << "Symbolic lambda: " << symbolic << "\n";
    
    // Apply with one concrete value and one hole
    auto partial_app = symbolic(Expr(3), hole("y"));
    std::cout << "Applied with x=3, y=?: " << partial_app << "\n";
    
    // Evaluate partially
    Computation comp(partial_app);
    auto simplified = comp.partial_eval();
    std::cout << "Simplified: " << simplified << "\n";
    
    // Fill remaining hole
    double result = comp.fill("y", 4).eval();
    std::cout << "(3 + 4)² = " << result << "\n";
}

int main() {
    std::cout << "================================================\n";
    std::cout << "   APERTURE FLUENT C++ API DEMO\n";
    std::cout << "================================================\n";
    
    demo_basic_arithmetic();
    demo_holes_and_partial_eval();
    demo_let_bindings();
    demo_lambda_functions();
    demo_conditionals();
    demo_secure_computation();
    demo_complex_expressions();
    demo_chaining_operations();
    demo_builder_pattern();
    demo_medical_dosage();
    demo_symbolic_math();
    
    std::cout << "\n================================================\n";
    std::cout << "   Fluent API Demo Complete!\n";
    std::cout << "================================================\n";
    
    return 0;
}