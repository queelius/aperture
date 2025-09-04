#include "../test_framework.hpp"
#include <cstdlib>

using namespace secure;
using namespace secure::test;

int main() {
    TestRunner runner(false);
    
    // Factorial-like computation (iterative simulation)
    runner.add_test("Factorial 5",
                   "(* 1 (* 2 (* 3 (* 4 5))))", 120.0);
    
    // Fibonacci-like (computing specific value)
    runner.add_test("Fibonacci 6th",
                   "(let ((f0 1) (f1 1)) "
                   "  (let ((f2 (+ f0 f1))) "
                   "    (let ((f3 (+ f1 f2))) "
                   "      (let ((f4 (+ f2 f3))) "
                   "        (let ((f5 (+ f3 f4))) "
                   "          f5)))))", 8.0);
    
    // Quadratic formula evaluation - discriminant
    runner.add_test("Quadratic discriminant",
                   "(let ((a 1) (b -5) (c 6)) "
                   "  (- (* b b) (* 4 (* a c))))", 1.0);
    
    // Distance formula squared (3-4-5 triangle)
    runner.add_test("Distance squared",
                   "(let ((x1 0) (y1 0) (x2 3) (y2 4)) "
                   "  (+ (* (- x2 x1) (- x2 x1)) "
                   "     (* (- y2 y1) (- y2 y1))))", 25.0);
    
    // Polynomial evaluation: 2x³ + 3x² - 4x + 5 at x=2
    runner.add_test("Polynomial eval",
                   "(let ((x 2)) "
                   "  (+ (* 2 (* x (* x x))) "
                   "     (+ (* 3 (* x x)) "
                   "        (+ (* -4 x) 5))))", 25.0);
    
    // Compound interest: P(1 + r)^n for n=3
    runner.add_test("Compound interest",
                   "(let ((p 1000) (r 0.05)) "
                   "  (let ((rate (+ 1 r))) "
                   "    (* p (* rate (* rate rate)))))", 1157.625);
    
    // Deep nesting
    runner.add_test("Deep nesting",
                   "(+ 1 (+ 2 (+ 3 (+ 4 (+ 5 (+ 6 7))))))", 28.0);
    
    // Many arguments
    runner.add_test("Many args",
                   "(+ 1 2 3 4 5 6 7 8 9 10)", 55.0);
    
    runner.run_all();
    
    return runner.all_passed() ? EXIT_SUCCESS : EXIT_FAILURE;
}