#include "../test_framework.hpp"
#include <cstdlib>

using namespace secure;
using namespace secure::test;

int main() {
    TestRunner runner(false);
    
    // Simple lambda
    runner.add_test("Lambda identity", "((lambda (x) x) 10)", 10.0);
    runner.add_test("Lambda add", "((lambda (x) (+ x 5)) 10)", 15.0);
    
    // Multiple parameters
    runner.add_test("Lambda multi-param", 
                   "((lambda (x y) (* x y)) 3 4)", 12.0);
    
    // Three parameters
    runner.add_test("Lambda three params",
                   "((lambda (x y z) (+ x (+ y z))) 1 2 3)", 6.0);
    
    // Closure over let binding
    runner.add_test("Lambda closure",
                   "(let ((x 10)) ((lambda (y) (+ x y)) 5))", 15.0);
    
    // Nested lambdas
    runner.add_test("Nested lambda",
                   "(((lambda (x) (lambda (y) (+ x y))) 10) 5)", 15.0);
    
    // Lambda in let
    runner.add_test("Lambda in let",
                   "(let ((f (lambda (x) (* x 2)))) (f 7))", 14.0);
    
    // Higher-order function
    runner.add_test("Higher order",
                   "(let ((apply (lambda (f x) (f x)))) "
                   "  (apply (lambda (y) (+ y 1)) 9))", 10.0);
    
    // Complex lambda body
    runner.add_test("Complex lambda body",
                   "((lambda (x) (* (+ x 1) (- x 1))) 5)", 24.0);
    
    runner.run_all();
    
    return runner.all_passed() ? EXIT_SUCCESS : EXIT_FAILURE;
}