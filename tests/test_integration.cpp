#include "../aperture.hpp"
#include <iostream>
#include <chrono>
#include <cstdlib>

using namespace secure;

bool test_complete_workflow() {
    std::cout << "\n=== Complete Workflow Test ===\n";
    
    Evaluator eval;
    
    // Complex expression with holes
    std::string expr_str = "(let ((rate 0.25) (base 1000)) "
                          "  (+ (* ?income rate) (* base 2) ?bonus))";
    Parser parser(expr_str);
    auto expr = parser.parse();
    
    if (!expr) {
        std::cout << "✗ Parse failed\n";
        return false;
    }
    
    // Partial evaluation
    auto partial = eval.partial_eval(*expr);
    if (!partial) {
        std::cout << "✗ Partial eval failed\n";
        return false;
    }
    
    // Find holes
    auto holes = eval.find_holes(*partial);
    if (holes.size() != 2) {
        std::cout << "✗ Expected 2 holes, found " << holes.size() << "\n";
        return false;
    }
    
    // Fill holes
    std::unordered_map<std::string, ValuePtr> values;
    values["income"] = num(50000);
    values["bonus"] = num(5000);
    
    auto filled = eval.fill_holes(*partial, values);
    if (!filled) {
        std::cout << "✗ Fill holes failed\n";
        return false;
    }
    
    // Final evaluation
    auto result = eval.eval(*filled);
    if (!result) {
        std::cout << "✗ Final eval failed\n";
        return false;
    }
    
    auto* n = std::get_if<Value::Num>(&(*result)->data);
    if (!n) {
        std::cout << "✗ Result is not a number\n";
        return false;
    }
    
    double expected = 50000 * 0.25 + 1000 * 2 + 5000;
    if (std::abs(n->val - expected) > 1e-9) {
        std::cout << "✗ Wrong result: " << n->val << " (expected " << expected << ")\n";
        return false;
    }
    
    std::cout << "✓ Complete workflow test passed\n";
    return true;
}

bool test_recursive_patterns() {
    std::cout << "\n=== Recursive Pattern Test ===\n";
    
    Evaluator eval;
    
    // Factorial-like recursive unrolling
    std::string factorial = "(let ((fact5 (* 5 (* 4 (* 3 (* 2 1)))))) fact5)";
    
    Parser p1(factorial);
    auto expr1 = p1.parse();
    if (!expr1) {
        std::cout << "✗ Factorial parse failed\n";
        return false;
    }
    
    auto result1 = eval.eval(*expr1);
    if (!result1) {
        std::cout << "✗ Factorial eval failed\n";
        return false;
    }
    
    if (auto* n = std::get_if<Value::Num>(&(*result1)->data)) {
        if (std::abs(n->val - 120) > 1e-9) {
            std::cout << "✗ Factorial wrong result: " << n->val << "\n";
            return false;
        }
    } else {
        std::cout << "✗ Factorial result not a number\n";
        return false;
    }
    
    std::cout << "✓ Recursive pattern test passed\n";
    return true;
}

bool test_security_properties() {
    std::cout << "\n=== Security Properties Test ===\n";
    
    Evaluator eval;
    
    // Test 1: Holes preserve privacy
    Parser p1("(* ?secret 2)");
    auto expr1 = p1.parse();
    if (!expr1) {
        std::cout << "✗ Parse failed\n";
        return false;
    }
    
    auto partial1 = eval.partial_eval(*expr1);
    if (!partial1) {
        std::cout << "✗ Partial eval failed\n";
        return false;
    }
    
    std::string partial_str = to_string(*partial1);
    if (partial_str.find("?secret") == std::string::npos) {
        std::cout << "✗ Secret not preserved in partial eval\n";
        return false;
    }
    
    // Test 2: Complex expression simplification preserves holes
    Parser p2("(let ((public 100)) (+ (* ?private public) (/ public 2)))");
    auto expr2 = p2.parse();
    if (!expr2) {
        std::cout << "✗ Complex parse failed\n";
        return false;
    }
    
    auto partial2 = eval.partial_eval(*expr2);
    if (!partial2) {
        std::cout << "✗ Complex partial eval failed\n";
        return false;
    }
    
    if (to_string(*partial2).find("?private") == std::string::npos) {
        std::cout << "✗ Private data not preserved\n";
        return false;
    }
    
    std::cout << "✓ Security properties test passed\n";
    return true;
}

bool test_performance() {
    std::cout << "\n=== Performance Test ===\n";
    
    Evaluator eval;
    
    // Build a deeply nested expression
    std::string deep = "(+";
    for (int i = 0; i < 100; i++) {
        deep += " (+ " + std::to_string(i) + " " + std::to_string(i + 1) + ")";
    }
    deep += ")";
    
    Parser p1(deep);
    auto expr = p1.parse();
    if (!expr) {
        std::cout << "✗ Deep parse failed\n";
        return false;
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    auto result = eval.eval(*expr);
    auto end = std::chrono::high_resolution_clock::now();
    
    if (!result) {
        std::cout << "✗ Deep eval failed\n";
        return false;
    }
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "  Deep nested eval took " << duration.count() << " microseconds\n";
    
    if (duration.count() > 10000) {  // 10ms threshold
        std::cout << "✗ Performance too slow\n";
        return false;
    }
    
    std::cout << "✓ Performance test passed\n";
    return true;
}

int main() {
    std::cout << "================================================\n";
    std::cout << "   APERTURE INTEGRATION TESTS\n";
    std::cout << "================================================\n";
    
    int passed = 0;
    int failed = 0;
    
    if (test_complete_workflow()) passed++; else failed++;
    if (test_recursive_patterns()) passed++; else failed++;
    if (test_security_properties()) passed++; else failed++;
    if (test_performance()) passed++; else failed++;
    
    std::cout << "\n================================================\n";
    std::cout << "Integration Test Results:\n";
    std::cout << "Passed: " << passed << "/" << (passed + failed) << "\n";
    std::cout << "Failed: " << failed << "/" << (passed + failed) << "\n";
    std::cout << "================================================\n";
    
    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}