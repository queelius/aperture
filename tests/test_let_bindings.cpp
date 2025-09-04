#include "../test_framework.hpp"
#include <cstdlib>

using namespace secure;
using namespace secure::test;

int main() {
    TestRunner runner(false);
    
    // Simple let - Python: x = 10; x + 5 = 15
    runner.add_test("Simple let", "(let ((x 10)) (+ x 5))", 15.0);
    
    // Multiple bindings - Python: x = 2; y = 3; x * y = 6
    runner.add_test("Multiple bindings", 
                   "(let ((x 2) (y 3)) (* x y))", 6.0);
    
    // Nested let - Python: x = 5; y = (x + 2); y * 2 = 14
    runner.add_test("Nested let", 
                   "(let ((x 5)) (let ((y (+ x 2))) (* y 2)))", 14.0);
    
    // Sequential binding - Python: a = 10; b = a * 2; a + b = 30
    runner.add_test("Sequential binding",
                   "(let ((a 10) (b (* a 2))) (+ a b))", 30.0);
    
    // Shadow binding - Inner x shadows outer x
    runner.add_test("Shadow binding",
                   "(let ((x 10)) (let ((x 5)) x))", 5.0);
    
    // Complex expression in binding
    runner.add_test("Complex binding",
                   "(let ((x (* (+ 2 3) 4))) (/ x 2))", 10.0);
    
    runner.run_all();
    
    return runner.all_passed() ? EXIT_SUCCESS : EXIT_FAILURE;
}