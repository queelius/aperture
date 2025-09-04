#include "../aperture.hpp"
#include <cstdlib>
#include <iostream>

using namespace secure;

int main() {
    Evaluator eval;
    int passed = 0;
    int failed = 0;
    
    // Test 1: Parse error (unclosed parenthesis)
    {
        Parser p("(+ 1 2");
        auto expr = p.parse();
        if (!expr) {
            std::cout << "✓ Parse error detected (unclosed paren)\n";
            passed++;
        } else {
            std::cout << "✗ Parse error not detected\n";
            failed++;
        }
    }
    
    // Test 2: Division by zero
    {
        Parser p("(/ 10 0)");
        auto expr = p.parse();
        if (expr) {
            auto result = eval.eval(*expr);
            if (!result && result.error().kind == ErrorKind::EVAL_ERROR) {
                std::cout << "✓ Division by zero detected\n";
                passed++;
            } else {
                std::cout << "✗ Division by zero not detected\n";
                failed++;
            }
        } else {
            std::cout << "✗ Failed to parse division expression\n";
            failed++;
        }
    }
    
    // Test 3: Undefined symbol
    {
        Parser p("(+ undefined_var 5)");
        auto expr = p.parse();
        if (expr) {
            auto result = eval.eval(*expr);
            if (!result && result.error().kind == ErrorKind::EVAL_ERROR) {
                std::cout << "✓ Undefined symbol detected\n";
                passed++;
            } else {
                std::cout << "✗ Undefined symbol not detected\n";
                failed++;
            }
        } else {
            std::cout << "✗ Failed to parse undefined symbol expression\n";
            failed++;
        }
    }
    
    // Test 4: Wrong number of arguments
    {
        Parser p("(- 5)");  // Subtraction requires exactly 2 args
        auto expr = p.parse();
        if (expr) {
            auto result = eval.eval(*expr);
            if (!result && result.error().kind == ErrorKind::EVAL_ERROR) {
                std::cout << "✓ Wrong arity detected\n";
                passed++;
            } else {
                std::cout << "✗ Wrong arity not detected\n";
                failed++;
            }
        } else {
            std::cout << "✗ Failed to parse arity test expression\n";
            failed++;
        }
    }
    
    // Test 5: Type error (string in arithmetic)
    {
        Parser p("(+ \"hello\" 5)");
        auto expr = p.parse();
        if (expr) {
            auto result = eval.eval(*expr);
            if (!result && result.error().kind == ErrorKind::TYPE_ERROR) {
                std::cout << "✓ Type error detected\n";
                passed++;
            } else {
                std::cout << "✗ Type error not detected\n";
                failed++;
            }
        } else {
            std::cout << "✗ Failed to parse type error expression\n";
            failed++;
        }
    }
    
    // Test 6: Hole access without permission
    {
        Parser p("(+ ?x 5)");
        auto expr = p.parse();
        if (expr) {
            auto result = eval.eval(*expr, false);  // Don't allow holes
            if (!result && result.error().kind == ErrorKind::HOLE_ACCESS) {
                std::cout << "✓ Hole access denied\n";
                passed++;
            } else {
                std::cout << "✗ Hole access not properly denied\n";
                failed++;
            }
        } else {
            std::cout << "✗ Failed to parse hole expression\n";
            failed++;
        }
    }
    
    std::cout << "\n=== Error Handling Test Results ===\n";
    std::cout << "Passed: " << passed << "/" << (passed + failed) << "\n";
    std::cout << "Failed: " << failed << "/" << (passed + failed) << "\n";
    
    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}