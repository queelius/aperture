#include "aperture.hpp"
#include <iostream>
#include <chrono>
#include <sstream>
#include <iomanip>

using namespace secure;

class IntegrationTest {
    std::string name;
    bool passed = true;
    std::stringstream details;
    
public:
    IntegrationTest(const std::string& n) : name(n) {
        std::cout << "\n=== " << name << " ===\n";
    }
    
    void assert_equals(const std::string& desc, double actual, double expected, double tolerance = 1e-9) {
        if (std::abs(actual - expected) > tolerance) {
            passed = false;
            details << "  ✗ " << desc << ": expected " << expected << ", got " << actual << "\n";
        } else {
            details << "  ✓ " << desc << "\n";
        }
    }
    
    void assert_contains(const std::string& desc, const std::string& str, const std::string& substr) {
        if (str.find(substr) == std::string::npos) {
            passed = false;
            details << "  ✗ " << desc << ": '" << substr << "' not found in '" << str << "'\n";
        } else {
            details << "  ✓ " << desc << "\n";
        }
    }
    
    void assert_true(const std::string& desc, bool condition) {
        if (!condition) {
            passed = false;
            details << "  ✗ " << desc << "\n";
        } else {
            details << "  ✓ " << desc << "\n";
        }
    }
    
    void report() {
        std::cout << details.str();
        if (passed) {
            std::cout << "✓ " << name << " passed\n";
        } else {
            std::cout << "✗ " << name << " failed\n";
        }
    }
    
    bool is_passed() const { return passed; }
};

// Test complete workflow: parse -> eval -> partial eval -> fill -> final eval
void test_complete_workflow() {
    IntegrationTest test("Complete Workflow Test");
    
    Evaluator eval;
    
    // Step 1: Parse complex expression with holes
    std::string expr_str = "(let ((rate 0.25) (base 1000)) "
                          "  (+ (* ?income rate) (* base 2) ?bonus))";
    Parser parser(expr_str);
    auto expr = parser.parse();
    
    test.assert_true("Parse succeeded", expr.has_value());
    
    // Step 2: Partial evaluation
    auto partial = eval.partial_eval(*expr);
    test.assert_true("Partial eval succeeded", partial.has_value());
    
    std::string partial_str = to_string(*partial);
    test.assert_contains("Partial contains income hole", partial_str, "?income");
    test.assert_contains("Partial contains bonus hole", partial_str, "?bonus");
    
    // Step 3: Find holes
    auto holes = eval.find_holes(*partial);
    test.assert_equals("Number of holes", holes.size(), 2);
    
    // Step 4: Fill holes
    std::unordered_map<std::string, ValuePtr> values;
    values["income"] = num(50000);
    values["bonus"] = num(5000);
    
    auto filled = eval.fill_holes(*partial, values);
    test.assert_true("Fill holes succeeded", filled.has_value());
    
    // Step 5: Final evaluation
    auto result = eval.eval(*filled);
    test.assert_true("Final eval succeeded", result.has_value());
    
    auto* n = std::get_if<Value::Num>(&(*result)->data);
    test.assert_true("Result is number", n != nullptr);
    test.assert_equals("Final result", n->val, 50000 * 0.25 + 1000 * 2 + 5000);
    
    test.report();
}

// Test recursive functions and complex control flow
void test_recursive_patterns() {
    IntegrationTest test("Recursive Pattern Test");
    
    Evaluator eval;
    
    // Factorial-like recursive unrolling (manual)
    std::string factorial = 
        "(let ((fact5 (* 5 (* 4 (* 3 (* 2 1))))))"
        "  fact5)";
    
    Parser p1(factorial);
    auto expr1 = p1.parse();
    auto result1 = eval.eval(*expr1);
    
    test.assert_true("Factorial parse succeeded", expr1.has_value());
    test.assert_true("Factorial eval succeeded", result1.has_value());
    
    if (auto* n = std::get_if<Value::Num>(&(*result1)->data)) {
        test.assert_equals("Factorial 5", n->val, 120);
    }
    
    // Nested conditionals simulating recursion base case
    std::string recursive_sim = 
        "(let ((n 3))"
        "  (if (- n 3)"
        "      (if (- n 2)"
        "          (if (- n 1)"
        "              1"
        "              2)"
        "          3)"
        "      6))";
    
    Parser p2(recursive_sim);
    auto expr2 = p2.parse();
    auto result2 = eval.eval(*expr2);
    
    test.assert_true("Recursive sim succeeded", result2.has_value());
    if (auto* n = std::get_if<Value::Num>(&(*result2)->data)) {
        test.assert_equals("Recursive sim result", n->val, 6);
    }
    
    test.report();
}

// Test error handling and recovery
void test_error_handling() {
    IntegrationTest test("Error Handling Test");
    
    Evaluator eval;
    
    // Test 1: Parse error
    Parser p1("(+ 1 2");  // Missing closing paren
    auto expr1 = p1.parse();
    test.assert_true("Parse error detected", !expr1.has_value());
    
    // Test 2: Undefined symbol
    Parser p2("(+ x 1)");
    auto expr2 = p2.parse();
    if (expr2) {
        auto result2 = eval.eval(*expr2);
        test.assert_true("Undefined symbol error", !result2.has_value());
    }
    
    // Test 3: Division by zero
    Parser p3("(/ 10 0)");
    auto expr3 = p3.parse();
    if (expr3) {
        auto result3 = eval.eval(*expr3);
        test.assert_true("Division by zero error", !result3.has_value());
    }
    
    // Test 4: Type error
    Parser p4("(+ \"hello\" 5)");
    auto expr4 = p4.parse();
    if (expr4) {
        auto result4 = eval.eval(*expr4);
        test.assert_true("Type error detected", !result4.has_value());
    }
    
    // Test 5: Hole access without permission
    Parser p5("(+ ?x 5)");
    auto expr5 = p5.parse();
    if (expr5) {
        auto result5 = eval.eval(*expr5, false);  // Don't allow holes
        test.assert_true("Hole access denied", !result5.has_value());
    }
    
    test.report();
}

// Test performance with complex expressions
void test_performance() {
    IntegrationTest test("Performance Test");
    
    Evaluator eval;
    
    // Build a deeply nested expression
    std::stringstream deep;
    deep << "(+";
    for (int i = 0; i < 100; i++) {
        deep << " (+ " << i << " " << (i + 1) << ")";
    }
    deep << ")";
    
    Parser p1(deep.str());
    auto expr = p1.parse();
    test.assert_true("Deep parse succeeded", expr.has_value());
    
    auto start = std::chrono::high_resolution_clock::now();
    auto result = eval.eval(*expr);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    test.assert_true("Deep eval succeeded", result.has_value());
    test.assert_true("Performance acceptable", duration.count() < 10000);  // < 10ms
    
    std::cout << "  Deep nested eval took " << duration.count() << " microseconds\n";
    
    // Test with many holes
    std::stringstream holes;
    holes << "(+";
    for (int i = 0; i < 50; i++) {
        holes << " ?x" << i;
    }
    holes << ")";
    
    Parser p2(holes.str());
    auto expr2 = p2.parse();
    
    start = std::chrono::high_resolution_clock::now();
    auto partial = eval.partial_eval(*expr2);
    end = std::chrono::high_resolution_clock::now();
    
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    test.assert_true("Many holes partial eval succeeded", partial.has_value());
    test.assert_true("Partial eval performance", duration.count() < 5000);  // < 5ms
    
    std::cout << "  Many holes partial eval took " << duration.count() << " microseconds\n";
    
    test.report();
}

// Test security properties
void test_security_properties() {
    IntegrationTest test("Security Properties Test");
    
    Evaluator eval;
    
    // Test 1: Holes preserve privacy
    Parser p1("(* ?secret 2)");
    auto expr1 = p1.parse();
    auto partial1 = eval.partial_eval(*expr1);
    
    test.assert_true("Secret remains hole", to_string(*partial1).find("?secret") != std::string::npos);
    
    // Test 2: Partial evaluation doesn't leak through operations
    Parser p2("(if ?condition ?private_true ?private_false)");
    auto expr2 = p2.parse();
    auto partial2 = eval.partial_eval(*expr2);
    
    std::string partial_str = to_string(*partial2);
    test.assert_contains("Condition preserved", partial_str, "?condition");
    test.assert_contains("True branch preserved", partial_str, "?private_true");
    test.assert_contains("False branch preserved", partial_str, "?private_false");
    
    // Test 3: Complex expression simplification preserves holes
    Parser p3("(let ((public 100)) (+ (* ?private public) (/ public 2)))");
    auto expr3 = p3.parse();
    auto partial3 = eval.partial_eval(*expr3);
    
    test.assert_true("Complex partial succeeded", partial3.has_value());
    test.assert_contains("Private preserved", to_string(*partial3), "?private");
    
    // Test 4: No information leakage through errors
    Parser p4("(/ ?secret 0)");  // Division by zero with hole
    auto expr4 = p4.parse();
    auto partial4 = eval.partial_eval(*expr4);
    
    // Should preserve the expression, not evaluate the division
    test.assert_true("Div by zero with hole succeeded", partial4.has_value());
    
    test.report();
}

// Test real-world scenario: Medical dosage calculation
void test_medical_scenario() {
    IntegrationTest test("Medical Dosage Scenario");
    
    Evaluator eval;
    
    // Complex dosage formula with patient data as holes
    std::string formula = R"(
        (let ((base_dose 10)
              (max_dose 50)
              (weight_factor 0.5)
              (age_factor (if (> ?age 65) 0.7 1.0))
              (condition_factor (+ 1 (* ?severity 0.5))))
          (let ((calculated (* base_dose 
                              (* (/ ?weight 70) weight_factor)
                              age_factor
                              condition_factor)))
            (if (> calculated max_dose) max_dose calculated)))
    )";
    
    Parser parser(formula);
    auto expr = parser.parse();
    test.assert_true("Formula parsed", expr.has_value());
    
    // Hospital processes formula (partial eval)
    auto optimized = eval.partial_eval(*expr);
    test.assert_true("Formula optimized", optimized.has_value());
    
    // Verify all patient data remains private
    auto holes = eval.find_holes(*optimized);
    test.assert_true("Age hole present", std::find(holes.begin(), holes.end(), "age") != holes.end());
    test.assert_true("Weight hole present", std::find(holes.begin(), holes.end(), "weight") != holes.end());
    test.assert_true("Severity hole present", std::find(holes.begin(), holes.end(), "severity") != holes.end());
    
    // Patient fills in data locally
    std::unordered_map<std::string, ValuePtr> patient_data;
    patient_data["age"] = num(45);
    patient_data["weight"] = num(80);
    patient_data["severity"] = num(0.6);
    
    auto filled = eval.fill_holes(*optimized, patient_data);
    test.assert_true("Data filled", filled.has_value());
    
    auto result = eval.eval(*filled);
    test.assert_true("Dosage calculated", result.has_value());
    
    if (auto* n = std::get_if<Value::Num>(&(*result)->data)) {
        // Expected: 10 * (80/70 * 0.5) * 1.0 * (1 + 0.6*0.5) = 10 * 0.571 * 1.0 * 1.3 = 7.43
        test.assert_equals("Dosage value", n->val, 10 * (80.0/70 * 0.5) * 1.0 * (1 + 0.6*0.5), 0.01);
    }
    
    test.report();
}

int main() {
    std::cout << "================================================\n";
    std::cout << "   APERTURE LANGUAGE INTEGRATION TESTS\n";
    std::cout << "================================================\n";
    
    test_complete_workflow();
    test_recursive_patterns();
    test_error_handling();
    test_performance();
    test_security_properties();
    test_medical_scenario();
    
    std::cout << "\n================================================\n";
    std::cout << "   Integration tests completed!\n";
    std::cout << "================================================\n";
    
    return 0;
}