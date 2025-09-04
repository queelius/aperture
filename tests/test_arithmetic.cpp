#include "../test_framework.hpp"
#include <cstdlib>

using namespace secure;
using namespace secure::test;

int main() {
    TestRunner runner(false);
    
    // Basic operations - compare with Python
    runner.add_test("Addition", "(+ 1 2)", 3.0);
    runner.add_test("Addition multiple", "(+ 1 2 3 4)", 10.0);
    runner.add_test("Subtraction", "(- 5 3)", 2.0);
    runner.add_test("Multiplication", "(* 3 4)", 12.0);
    runner.add_test("Division", "(/ 10 2)", 5.0);
    
    // Nested operations
    runner.add_test("Nested 1", "(* (+ 2 3) 4)", 20.0);
    runner.add_test("Nested 2", "(/ 10 (+ 2 3))", 2.0);
    runner.add_test("Complex nested", "(/ (+ (* 3 4) (* 5 6)) 2)", 21.0);
    
    // Floating point
    runner.add_test("Float addition", "(+ 1.5 2.5)", 4.0);
    runner.add_test("Float division", "(/ 7.5 2.5)", 3.0);
    
    // Edge cases
    runner.add_test("Add zero", "(+ 0 5)", 5.0);
    runner.add_test("Multiply by zero", "(* 100 0)", 0.0);
    runner.add_test("Negative numbers", "(+ -5 3)", -2.0);
    runner.add_test("Negative multiplication", "(* -3 -4)", 12.0);
    
    // Order of operations
    runner.add_test("Order 1", "(+ 2 (* 3 4))", 14.0);
    
    // Scientific notation
    runner.add_test("Large number", "(+ 1e10 1e10)", 2e10);
    runner.add_test("Small number", "(* 1e-10 2)", 2e-10);
    
    // Error cases
    runner.add_error_test("Division by zero", "(/ 5 0)");
    runner.add_error_test("Wrong arity sub", "(- 5)");
    runner.add_error_test("Wrong arity div", "(/ 5)");
    
    runner.run_all();
    
    return runner.all_passed() ? EXIT_SUCCESS : EXIT_FAILURE;
}