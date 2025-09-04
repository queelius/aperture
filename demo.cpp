#include "aperture.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>

using namespace secure;

void print_result(const std::string& label, const ValuePtr& val) {
    std::cout << std::left << std::setw(30) << label << ": " << to_string(val) << "\n";
}

void demo_basic_evaluation() {
    std::cout << "\n=== Basic Evaluation ===\n";
    
    Evaluator eval;
    Parser parser("(+ 1 2 3)");
    
    auto expr = parser.parse();
    if (!expr) {
        std::cerr << "Parse error: " << expr.error().message << "\n";
        return;
    }
    
    auto result = eval.eval(*expr);
    if (!result) {
        std::cerr << "Eval error: " << result.error().message << "\n";
        return;
    }
    
    print_result("(+ 1 2 3)", *result);
}

void demo_partial_evaluation() {
    std::cout << "\n=== Partial Evaluation with Holes ===\n";
    
    Evaluator eval;
    
    // Expression with holes: (+ (* ?x 2) (* ?y 3) 10)
    Parser parser("(+ (* ?x 2) (* ?y 3) 10)");
    auto expr = parser.parse();
    if (!expr) {
        std::cerr << "Parse error\n";
        return;
    }
    
    print_result("Original", *expr);
    
    // Partial evaluation - simplifies what it can
    auto partial = eval.partial_eval(*expr);
    if (!partial) {
        std::cerr << "Partial eval error\n";
        return;
    }
    
    print_result("After partial eval", *partial);
    
    // Find holes
    auto holes = eval.find_holes(*partial);
    std::cout << "Holes found: ";
    for (const auto& h : holes) {
        std::cout << h << " ";
    }
    std::cout << "\n";
    
    // Fill some holes
    std::unordered_map<std::string, ValuePtr> values;
    values["x"] = num(5);
    
    auto filled = eval.fill_holes(*partial, values);
    if (!filled) {
        std::cerr << "Fill error\n";
        return;
    }
    
    print_result("After filling x=5", *filled);
    
    // Evaluate with x filled
    auto partial2 = eval.partial_eval(*filled);
    if (!partial2) {
        std::cerr << "Eval error\n";
        return;
    }
    
    print_result("After evaluating with x", *partial2);
    
    // Fill remaining hole
    values["y"] = num(7);
    auto filled2 = eval.fill_holes(*partial2, values);
    if (!filled2) {
        std::cerr << "Fill error\n";
        return;
    }
    
    auto final_result = eval.eval(*filled2);
    if (!final_result) {
        std::cerr << "Final eval error\n";
        return;
    }
    
    print_result("Final result", *final_result);
}

void demo_secure_computation() {
    std::cout << "\n=== Secure Distributed Computation ===\n";
    std::cout << "Scenario: Tax calculation where income is confidential\n\n";
    
    Evaluator untrusted;  // Untrusted node
    Evaluator trusted;    // Trusted client
    
    // Tax calculation: tax = income * rate - deductions
    // Public: rate = 0.25, deductions = 5000
    // Private: income
    
    Parser parser("(let ((rate 0.25) (deductions 5000)) (- (* ?income rate) deductions))");
    auto expr = parser.parse();
    if (!expr) {
        std::cerr << "Parse error\n";
        return;
    }
    
    std::cout << "Expression sent to untrusted node:\n";
    print_result("Original", *expr);
    
    // Untrusted node performs partial evaluation
    auto simplified = untrusted.partial_eval(*expr);
    if (!simplified) {
        std::cerr << "Simplification error\n";
        return;
    }
    
    std::cout << "\nUntrusted node returns simplified expression:\n";
    print_result("Simplified", *simplified);
    
    // Client verifies holes
    auto holes = trusted.find_holes(*simplified);
    std::cout << "\nClient verifies holes: ";
    for (const auto& h : holes) {
        std::cout << h << " ";
    }
    std::cout << "\n";
    
    // Client fills confidential data locally
    std::unordered_map<std::string, ValuePtr> private_data;
    private_data["income"] = num(100000);
    
    auto filled = trusted.fill_holes(*simplified, private_data);
    if (!filled) {
        std::cerr << "Fill error\n";
        return;
    }
    
    std::cout << "\nClient fills income locally and evaluates:\n";
    auto final_result = trusted.eval(*filled);
    if (!final_result) {
        std::cerr << "Eval error\n";
        return;
    }
    
    print_result("Tax owed", *final_result);
}

void demo_lambda_with_holes() {
    std::cout << "\n=== Lambda Functions with Holes ===\n";
    
    Evaluator eval;
    
    // Define a function with holes in its body
    Parser parser("((lambda (x) (+ x ?secret)) 10)");
    auto expr = parser.parse();
    if (!expr) {
        std::cerr << "Parse error\n";
        return;
    }
    
    print_result("Original", *expr);
    
    // Partial evaluation
    auto partial = eval.partial_eval(*expr);
    if (!partial) {
        std::cerr << "Partial eval error\n";
        return;
    }
    
    print_result("After partial eval", *partial);
    
    // Fill the hole
    std::unordered_map<std::string, ValuePtr> values;
    values["secret"] = num(42);
    
    auto filled = eval.fill_holes(*partial, values);
    if (!filled) {
        std::cerr << "Fill error\n";
        return;
    }
    
    auto result = eval.eval(*filled);
    if (!result) {
        std::cerr << "Eval error\n";
        return;
    }
    
    print_result("Final result", *result);
}

void demo_complex_expression() {
    std::cout << "\n=== Complex Expression with Multiple Holes ===\n";
    
    Evaluator eval;
    
    // Quadratic formula: ax^2 + bx + c where x is private
    Parser parser("(let ((a 2) (c 3)) (+ (* a (* ?x ?x)) (* ?b ?x) c))");
    auto expr = parser.parse();
    if (!expr) {
        std::cerr << "Parse error\n";
        return;
    }
    
    print_result("Original quadratic", *expr);
    
    // Partial evaluation - simplifies known values
    auto partial = eval.partial_eval(*expr);
    if (!partial) {
        std::cerr << "Partial eval error\n";
        return;
    }
    
    print_result("Simplified", *partial);
    
    // Fill b coefficient (public)
    std::unordered_map<std::string, ValuePtr> public_values;
    public_values["b"] = num(5);
    
    auto with_b = eval.fill_holes(*partial, public_values);
    if (!with_b) {
        std::cerr << "Fill error\n";
        return;
    }
    
    auto simplified2 = eval.partial_eval(*with_b);
    if (!simplified2) {
        std::cerr << "Eval error\n";
        return;
    }
    
    print_result("With b=5", *simplified2);
    
    // Client fills private x value
    std::unordered_map<std::string, ValuePtr> private_values;
    private_values["x"] = num(4);
    
    auto filled = eval.fill_holes(*simplified2, private_values);
    if (!filled) {
        std::cerr << "Fill error\n";
        return;
    }
    
    auto result = eval.eval(*filled);
    if (!result) {
        std::cerr << "Eval error\n";
        return;
    }
    
    print_result("f(4) = 2*16 + 5*4 + 3", *result);
}

void benchmark_evaluation() {
    std::cout << "\n=== Performance Benchmark ===\n";
    
    Evaluator eval;
    const int iterations = 10000;
    
    // Test expression
    Parser parser("(+ (* 2 3) (* 4 5) (- 10 2))");
    auto expr = parser.parse();
    if (!expr) return;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; i++) {
        auto result = eval.eval(*expr);
        if (!result) {
            std::cerr << "Eval error\n";
            return;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Evaluated " << iterations << " expressions in " 
              << duration.count() << " microseconds\n";
    std::cout << "Average: " << (duration.count() / iterations) << " microseconds per evaluation\n";
    
    // Benchmark with holes
    Parser parser2("(+ (* ?x 3) (* 4 ?y) (- 10 2))");
    auto expr2 = parser2.parse();
    if (!expr2) return;
    
    start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; i++) {
        auto result = eval.partial_eval(*expr2);
        if (!result) {
            std::cerr << "Partial eval error\n";
            return;
        }
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "\nPartial evaluation with holes:\n";
    std::cout << "Evaluated " << iterations << " expressions in " 
              << duration.count() << " microseconds\n";
    std::cout << "Average: " << (duration.count() / iterations) << " microseconds per evaluation\n";
}

int main() {
    std::cout << "==============================================\n";
    std::cout << "    Secure Aperture Language Demo\n";
    std::cout << "==============================================\n";
    
    demo_basic_evaluation();
    demo_partial_evaluation();
    demo_secure_computation();
    demo_lambda_with_holes();
    demo_complex_expression();
    benchmark_evaluation();
    
    std::cout << "\n==============================================\n";
    std::cout << "Demo completed successfully!\n";
    
    return 0;
}