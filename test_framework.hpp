#pragma once
#include "aperture.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <iomanip>

namespace secure::test {

class TestRunner {
public:
    struct TestCase {
        std::string name;
        std::string aperture_code;
        double expected_result;
        bool should_error = false;
        double tolerance = 1e-9;
    };
    
private:
    std::vector<TestCase> tests;
    int passed = 0;
    int failed = 0;
    bool verbose = false;
    
public:
    TestRunner(bool verbose = false) : verbose(verbose) {}
    
    void add_test(const std::string& name, const std::string& aperture_code, 
                  double expected, double tolerance = 1e-9) {
        tests.push_back({name, aperture_code, expected, false, tolerance});
    }
    
    void add_error_test(const std::string& name, const std::string& aperture_code) {
        tests.push_back({name, aperture_code, 0, true, 0});
    }
    
    bool run_test(const TestCase& test) {
        if (verbose) {
            std::cout << "  Running: " << test.name << "\n";
            std::cout << "    Code: " << test.aperture_code << "\n";
        }
        
        Evaluator eval;
        Parser parser(test.aperture_code);
        
        auto expr = parser.parse();
        if (!expr) {
            if (test.should_error) {
                if (verbose) std::cout << "    ✓ Expected parse error occurred\n";
                return true;
            }
            std::cerr << "    ✗ Parse error: " << expr.error().message << "\n";
            return false;
        }
        
        auto result = eval.eval(*expr);
        if (!result) {
            if (test.should_error) {
                if (verbose) std::cout << "    ✓ Expected eval error occurred\n";
                return true;
            }
            std::cerr << "    ✗ Eval error: " << result.error().message << "\n";
            return false;
        }
        
        if (test.should_error) {
            std::cerr << "    ✗ Expected error but evaluation succeeded\n";
            return false;
        }
        
        // Extract numeric result
        auto* num_val = std::get_if<Value::Num>(&(*result)->data);
        if (!num_val) {
            std::cerr << "    ✗ Result is not a number: " << to_string(*result) << "\n";
            return false;
        }
        
        double actual = num_val->val;
        bool passed = std::abs(actual - test.expected_result) <= test.tolerance;
        
        if (verbose || !passed) {
            std::cout << "    Expected: " << test.expected_result 
                     << ", Got: " << actual;
            if (passed) {
                std::cout << " ✓\n";
            } else {
                std::cout << " ✗ (diff: " 
                         << std::abs(actual - test.expected_result) << ")\n";
            }
        }
        
        return passed;
    }
    
    void run_all() {
        std::cout << "\n=== Running Test Suite ===\n";
        
        for (const auto& test : tests) {
            if (run_test(test)) {
                passed++;
                if (!verbose) std::cout << ".";
            } else {
                failed++;
                if (!verbose) {
                    std::cout << "F";
                    std::cout << "\nFailed: " << test.name << "\n";
                }
            }
        }
        
        std::cout << "\n\n=== Test Results ===\n";
        std::cout << "Passed: " << passed << "/" << (passed + failed) << "\n";
        std::cout << "Failed: " << failed << "/" << (passed + failed) << "\n";
        
        if (failed == 0) {
            std::cout << "✓ All tests passed!\n";
        } else {
            std::cout << "✗ Some tests failed\n";
        }
    }
    
    bool all_passed() const { return failed == 0; }
};

// Helper class for partial evaluation tests
class PartialEvalTester {
    Evaluator eval;
    
public:
    struct HoleTest {
        std::string name;
        std::string expr_with_holes;
        std::unordered_map<std::string, double> hole_values;
        double expected_result;
        std::string expected_partial;  // Expected partial evaluation result
    };
    
    bool test_partial_eval(const HoleTest& test, bool verbose = false) {
        if (verbose) {
            std::cout << "Testing: " << test.name << "\n";
        }
        
        Parser parser(test.expr_with_holes);
        auto expr = parser.parse();
        if (!expr) {
            std::cerr << "Parse error in " << test.name << "\n";
            return false;
        }
        
        // Test partial evaluation
        auto partial = eval.partial_eval(*expr);
        if (!partial) {
            std::cerr << "Partial eval error in " << test.name << "\n";
            return false;
        }
        
        if (verbose) {
            std::cout << "  Partial result: " << to_string(*partial) << "\n";
            std::cout << "  Expected: " << test.expected_partial << "\n";
        }
        
        // Fill holes and evaluate
        std::unordered_map<std::string, ValuePtr> values;
        for (const auto& [key, val] : test.hole_values) {
            values[key] = num(val);
        }
        
        auto filled = eval.fill_holes(*partial, values);
        if (!filled) {
            std::cerr << "Fill error in " << test.name << "\n";
            return false;
        }
        
        auto final_result = eval.eval(*filled);
        if (!final_result) {
            std::cerr << "Final eval error in " << test.name << "\n";
            return false;
        }
        
        auto* num_val = std::get_if<Value::Num>(&(*final_result)->data);
        if (!num_val) {
            std::cerr << "Result not a number in " << test.name << "\n";
            return false;
        }
        
        bool passed = std::abs(num_val->val - test.expected_result) < 1e-9;
        if (verbose || !passed) {
            std::cout << "  Final result: " << num_val->val 
                     << " (expected: " << test.expected_result << ")\n";
        }
        
        return passed;
    }
};

} // namespace secure::test