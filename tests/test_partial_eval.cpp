#include "../test_framework.hpp"
#include "../aperture.hpp"
#include <cstdlib>
#include <iostream>

using namespace secure;

int main() {
    Evaluator eval;
    int passed = 0;
    int failed = 0;
    
    // Test 1: Simple arithmetic with hole
    {
        Parser p("(+ 10 ?x 5)");
        auto expr = p.parse();
        if (expr) {
            auto partial = eval.partial_eval(*expr);
            if (partial) {
                auto holes = eval.find_holes(*partial);
                if (holes.size() == 1 && holes[0] == "x") {
                    // Fill and evaluate
                    std::unordered_map<std::string, ValuePtr> values{{"x", num(20)}};
                    auto filled = eval.fill_holes(*partial, values);
                    if (filled) {
                        auto result = eval.eval(*filled);
                        if (result && std::get_if<Value::Num>(&(*result)->data)) {
                            auto val = std::get_if<Value::Num>(&(*result)->data)->val;
                            if (std::abs(val - 35.0) < 1e-9) {
                                std::cout << "✓ Simple addition with hole\n";
                                passed++;
                            } else {
                                std::cout << "✗ Simple addition with hole: wrong value\n";
                                failed++;
                            }
                        } else {
                            std::cout << "✗ Simple addition with hole: eval failed\n";
                            failed++;
                        }
                    } else {
                        std::cout << "✗ Simple addition with hole: fill failed\n";
                        failed++;
                    }
                } else {
                    std::cout << "✗ Simple addition with hole: wrong holes\n";
                    failed++;
                }
            } else {
                std::cout << "✗ Simple addition with hole: partial eval failed\n";
                failed++;
            }
        } else {
            std::cout << "✗ Simple addition with hole: parse failed\n";
            failed++;
        }
    }
    
    // Test 2: Lambda with hole in body
    {
        Parser p("((lambda (x) (+ x ?secret)) 10)");
        auto expr = p.parse();
        if (expr) {
            auto partial = eval.partial_eval(*expr);
            if (partial) {
                std::unordered_map<std::string, ValuePtr> values{{"secret", num(32)}};
                auto filled = eval.fill_holes(*partial, values);
                if (filled) {
                    auto result = eval.eval(*filled);
                    if (result && std::get_if<Value::Num>(&(*result)->data)) {
                        auto val = std::get_if<Value::Num>(&(*result)->data)->val;
                        if (std::abs(val - 42.0) < 1e-9) {
                            std::cout << "✓ Lambda with hole\n";
                            passed++;
                        } else {
                            std::cout << "✗ Lambda with hole: wrong value\n";
                            failed++;
                        }
                    } else {
                        std::cout << "✗ Lambda with hole: eval failed\n";
                        failed++;
                    }
                } else {
                    std::cout << "✗ Lambda with hole: fill failed\n";
                    failed++;
                }
            } else {
                std::cout << "✗ Lambda with hole: partial eval failed\n";
                failed++;
            }
        } else {
            std::cout << "✗ Lambda with hole: parse failed\n";
            failed++;
        }
    }
    
    // Test 3: Conditional with hole
    {
        Parser p("(if ?cond 100 200)");
        auto expr = p.parse();
        if (expr) {
            auto partial = eval.partial_eval(*expr);
            if (partial) {
                // Test true condition
                std::unordered_map<std::string, ValuePtr> values{{"cond", num(1)}};
                auto filled = eval.fill_holes(*partial, values);
                if (filled) {
                    auto result = eval.eval(*filled);
                    if (result && std::get_if<Value::Num>(&(*result)->data)) {
                        auto val = std::get_if<Value::Num>(&(*result)->data)->val;
                        if (std::abs(val - 100.0) < 1e-9) {
                            std::cout << "✓ Conditional with hole (true)\n";
                            passed++;
                        } else {
                            std::cout << "✗ Conditional with hole: wrong value\n";
                            failed++;
                        }
                    } else {
                        std::cout << "✗ Conditional with hole: eval failed\n";
                        failed++;
                    }
                } else {
                    std::cout << "✗ Conditional with hole: fill failed\n";
                    failed++;
                }
            } else {
                std::cout << "✗ Conditional with hole: partial eval failed\n";
                failed++;
            }
        } else {
            std::cout << "✗ Conditional with hole: parse failed\n";
            failed++;
        }
    }
    
    std::cout << "\n=== Partial Evaluation Test Results ===\n";
    std::cout << "Passed: " << passed << "/" << (passed + failed) << "\n";
    std::cout << "Failed: " << failed << "/" << (passed + failed) << "\n";
    
    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}