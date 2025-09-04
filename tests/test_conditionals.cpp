#include "../test_framework.hpp"
#include <cstdlib>

using namespace secure;
using namespace secure::test;

int main() {
    TestRunner runner(false);
    
    // Basic if - Python: 5 if 1 else 10
    runner.add_test("If true", "(if 1 5 10)", 5.0);
    runner.add_test("If false", "(if 0 5 10)", 10.0);
    
    // Nil is false
    runner.add_test("If nil", "(if nil 5 10)", 10.0);
    
    // Any non-zero number is true
    runner.add_test("If negative", "(if -1 5 10)", 5.0);
    
    // Computed condition
    runner.add_test("If computed", "(if (- 3 2) 5 10)", 5.0);
    
    // Nested if
    runner.add_test("Nested if", "(if 1 (if 1 5 10) 20)", 5.0);
    
    // If with complex expressions
    runner.add_test("If complex", "(if (- 5 5) (* 2 3) (+ 4 6))", 10.0);
    
    // If in let binding
    runner.add_test("If in let", 
                   "(let ((x (if 1 5 10))) (* x 2))", 10.0);
    
    runner.run_all();
    
    return runner.all_passed() ? EXIT_SUCCESS : EXIT_FAILURE;
}