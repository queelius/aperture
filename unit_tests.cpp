#include "test_framework.hpp"
#include <cmath>

using namespace secure;
using namespace secure::test;

void test_basic_arithmetic() {
    TestRunner runner(false);
    
    // Basic operations - compare with Python
    runner.add_test("Addition", "(+ 1 2)", 3.0);
    runner.add_test("Addition multiple", "(+ 1 2 3 4)", 10.0);
    runner.add_test("Subtraction", "(- 5 3)", 2.0);
    runner.add_test("Multiplication", "(* 3 4)", 12.0);
    runner.add_test("Division", "(/ 10 2)", 5.0);
    
    // Nested operations - Python: (2 + 3) * 4 = 20
    runner.add_test("Nested 1", "(* (+ 2 3) 4)", 20.0);
    
    // Python: 10 / (2 + 3) = 2
    runner.add_test("Nested 2", "(/ 10 (+ 2 3))", 2.0);
    
    // Python: ((3 * 4) + (5 * 6)) / 2 = 21
    runner.add_test("Complex nested", "(/ (+ (* 3 4) (* 5 6)) 2)", 21.0);
    
    // Floating point - Python: 1.5 + 2.5 = 4.0
    runner.add_test("Float addition", "(+ 1.5 2.5)", 4.0);
    
    // Python: 7.5 / 2.5 = 3.0
    runner.add_test("Float division", "(/ 7.5 2.5)", 3.0);
    
    // Edge cases
    runner.add_test("Add zero", "(+ 0 5)", 5.0);
    runner.add_test("Multiply by zero", "(* 100 0)", 0.0);
    runner.add_test("Negative numbers", "(+ -5 3)", -2.0);
    runner.add_test("Negative multiplication", "(* -3 -4)", 12.0);
    
    // Order of operations - Python: 2 + 3 * 4 evaluated left to right in S-expr
    runner.add_test("Order 1", "(+ 2 (* 3 4))", 14.0);
    
    // Error cases
    runner.add_error_test("Division by zero", "(/ 5 0)");
    runner.add_error_test("Wrong arity sub", "(- 5)");
    runner.add_error_test("Wrong arity div", "(/ 5)");
    
    std::cout << "\n### Arithmetic Tests ###\n";
    runner.run_all();
}

void test_let_bindings() {
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
    // Python: x = 10; def f(): x = 5; return x; f() = 5
    runner.add_test("Shadow binding",
                   "(let ((x 10)) (let ((x 5)) x))", 5.0);
    
    // Complex expression in binding
    // Python: x = (2 + 3) * 4; x / 2 = 10
    runner.add_test("Complex binding",
                   "(let ((x (* (+ 2 3) 4))) (/ x 2))", 10.0);
    
    std::cout << "\n### Let Binding Tests ###\n";
    runner.run_all();
}

void test_conditionals() {
    TestRunner runner(false);
    
    // Basic if - Python: 5 if 1 else 10 = 5
    runner.add_test("If true", "(if 1 5 10)", 5.0);
    
    // Python: 5 if 0 else 10 = 10
    runner.add_test("If false", "(if 0 5 10)", 10.0);
    
    // Nil is false - Python: 5 if None else 10 = 10
    runner.add_test("If nil", "(if nil 5 10)", 10.0);
    
    // Any non-zero number is true - Python: 5 if -1 else 10 = 5
    runner.add_test("If negative", "(if -1 5 10)", 5.0);
    
    // Computed condition - Python: 5 if (3 > 2) else 10
    // Since we don't have > yet, use arithmetic trick
    runner.add_test("If computed", "(if (- 3 2) 5 10)", 5.0);
    
    // Nested if - Python: (5 if 1 else 10) if 1 else 20 = 5
    runner.add_test("Nested if", "(if 1 (if 1 5 10) 20)", 5.0);
    
    // If with complex expressions
    // Python: (2 * 3) if (5 - 5) else (4 + 6) = 10
    runner.add_test("If complex", "(if (- 5 5) (* 2 3) (+ 4 6))", 10.0);
    
    // If in let binding
    // Python: x = 5 if 1 else 10; x * 2 = 10
    runner.add_test("If in let", 
                   "(let ((x (if 1 5 10))) (* x 2))", 10.0);
    
    std::cout << "\n### Conditional Tests ###\n";
    runner.run_all();
}

void test_lambda_functions() {
    TestRunner runner(false);
    
    // Simple lambda - Python: (lambda x: x + 5)(10) = 15
    runner.add_test("Lambda identity", "((lambda (x) x) 10)", 10.0);
    runner.add_test("Lambda add", "((lambda (x) (+ x 5)) 10)", 15.0);
    
    // Multiple parameters - Python: (lambda x, y: x * y)(3, 4) = 12
    runner.add_test("Lambda multi-param", 
                   "((lambda (x y) (* x y)) 3 4)", 12.0);
    
    // Lambda with three params - Python: (lambda x,y,z: x+y+z)(1,2,3) = 6
    runner.add_test("Lambda three params",
                   "((lambda (x y z) (+ x (+ y z))) 1 2 3)", 6.0);
    
    // Closure over let binding
    // Python: x = 10; f = lambda y: x + y; f(5) = 15
    runner.add_test("Lambda closure",
                   "(let ((x 10)) ((lambda (y) (+ x y)) 5))", 15.0);
    
    // Nested lambdas - Python: (lambda x: (lambda y: x + y))(10)(5) = 15
    runner.add_test("Nested lambda",
                   "(((lambda (x) (lambda (y) (+ x y))) 10) 5)", 15.0);
    
    // Lambda in let - Python: f = lambda x: x * 2; f(7) = 14
    runner.add_test("Lambda in let",
                   "(let ((f (lambda (x) (* x 2)))) (f 7))", 14.0);
    
    // Higher-order function
    // Python: apply = lambda f, x: f(x); apply(lambda y: y+1, 9) = 10
    runner.add_test("Higher order",
                   "(let ((apply (lambda (f x) (f x)))) "
                   "  (apply (lambda (y) (+ y 1)) 9))", 10.0);
    
    // Complex lambda body
    // Python: (lambda x: (x + 1) * (x - 1))(5) = 24
    runner.add_test("Complex lambda body",
                   "((lambda (x) (* (+ x 1) (- x 1))) 5)", 24.0);
    
    std::cout << "\n### Lambda Function Tests ###\n";
    runner.run_all();
}

void test_complex_programs() {
    TestRunner runner(false);
    
    // Factorial-like computation (iterative simulation)
    // Python: x=5; result=1*2*3*4*5 = 120
    runner.add_test("Factorial 5",
                   "(* 1 (* 2 (* 3 (* 4 5))))", 120.0);
    
    // Fibonacci-like (computing specific value)
    // Python: fib(6) = 8 (sequence: 1,1,2,3,5,8)
    // Simulating: f0=1, f1=1, f2=2, f3=3, f4=5, f5=8
    runner.add_test("Fibonacci 6th",
                   "(let ((f0 1) (f1 1)) "
                   "  (let ((f2 (+ f0 f1))) "
                   "    (let ((f3 (+ f1 f2))) "
                   "      (let ((f4 (+ f2 f3))) "
                   "        (let ((f5 (+ f3 f4))) "
                   "          f5)))))", 8.0);
    
    // Quadratic formula evaluation
    // Python: a=1, b=-5, c=6; x = 2 (one root)
    // Checking discriminant: b²-4ac = 25-24 = 1
    runner.add_test("Quadratic discriminant",
                   "(let ((a 1) (b -5) (c 6)) "
                   "  (- (* b b) (* 4 (* a c))))", 1.0);
    
    // Distance formula: sqrt((x2-x1)² + (y2-y1)²)
    // Points (0,0) to (3,4) = 5 (3-4-5 triangle)
    // Without sqrt, we calculate the squared distance = 25
    runner.add_test("Distance squared",
                   "(let ((x1 0) (y1 0) (x2 3) (y2 4)) "
                   "  (+ (* (- x2 x1) (- x2 x1)) "
                   "     (* (- y2 y1) (- y2 y1))))", 25.0);
    
    // Polynomial evaluation: 2x³ + 3x² - 4x + 5 at x=2
    // Python: 2*8 + 3*4 - 4*2 + 5 = 16 + 12 - 8 + 5 = 25
    runner.add_test("Polynomial eval",
                   "(let ((x 2)) "
                   "  (+ (* 2 (* x (* x x))) "
                   "     (+ (* 3 (* x x)) "
                   "        (+ (* -4 x) 5))))", 25.0);
    
    // Compound interest: P(1 + r)^n, simplified for n=3
    // Python: 1000 * 1.05 * 1.05 * 1.05 = 1157.625
    runner.add_test("Compound interest",
                   "(let ((p 1000) (r 0.05)) "
                   "  (let ((rate (+ 1 r))) "
                   "    (* p (* rate (* rate rate)))))", 1157.625);
    
    std::cout << "\n### Complex Program Tests ###\n";
    runner.run_all();
}

void test_partial_evaluation() {
    PartialEvalTester tester;
    std::vector<PartialEvalTester::HoleTest> tests;
    
    // Simple arithmetic with hole
    tests.push_back({
        "Simple addition with hole",
        "(+ 10 ?x 5)",
        {{"x", 20}},
        35.0,
        "(+ 15 ?x)"
    });
    
    // Multiple holes
    tests.push_back({
        "Multiple holes",
        "(+ ?x (* ?y 2))",
        {{"x", 10}, {"y", 5}},
        20.0,
        "(+ ?x (* ?y 2))"
    });
    
    // Partial evaluation with constants
    tests.push_back({
        "Simplify with constants",
        "(* (+ 2 3) ?x)",
        {{"x", 4}},
        20.0,
        "(* 5 ?x)"
    });
    
    // Nested expression with holes
    tests.push_back({
        "Nested with holes",
        "(let ((a 5)) (+ a ?x (* ?y 2)))",
        {{"x", 10}, {"y", 3}},
        21.0,
        "(+ 5 ?x (* ?y 2))"
    });
    
    // Conditional with hole in condition
    tests.push_back({
        "If with hole condition",
        "(if ?cond 100 200)",
        {{"cond", 1}},
        100.0,
        "(if ?cond 100 200)"
    });
    
    // Lambda with hole in body
    tests.push_back({
        "Lambda with hole",
        "((lambda (x) (+ x ?secret)) 10)",
        {{"secret", 32}},
        42.0,
        "(+ 10 ?secret)"
    });
    
    std::cout << "\n### Partial Evaluation Tests ###\n";
    
    int passed = 0;
    int failed = 0;
    
    for (const auto& test : tests) {
        if (tester.test_partial_eval(test, false)) {
            passed++;
            std::cout << ".";
        } else {
            failed++;
            std::cout << "F";
            std::cout << "\nFailed: " << test.name << "\n";
        }
    }
    
    std::cout << "\n\n=== Partial Eval Results ===\n";
    std::cout << "Passed: " << passed << "/" << (passed + failed) << "\n";
    std::cout << "Failed: " << failed << "/" << (passed + failed) << "\n";
}

void test_edge_cases() {
    TestRunner runner(false);
    
    // Empty list evaluates to nil which we treat as 0 in numeric context
    // But nil itself is not a number, so we skip this test
    
    // Single number
    runner.add_test("Single number", "42", 42.0);
    
    // Very large numbers
    runner.add_test("Large number", "(+ 1e10 1e10)", 2e10);
    
    // Very small numbers
    runner.add_test("Small number", "(* 1e-10 2)", 2e-10);
    
    // Deep nesting
    runner.add_test("Deep nesting",
                   "(+ 1 (+ 2 (+ 3 (+ 4 (+ 5 (+ 6 7))))))", 28.0);
    
    // Many arguments
    runner.add_test("Many args",
                   "(+ 1 2 3 4 5 6 7 8 9 10)", 55.0);
    
    std::cout << "\n### Edge Case Tests ###\n";
    runner.run_all();
}

int main() {
    std::cout << "================================================\n";
    std::cout << "   APERTURE LANGUAGE UNIT TESTS\n";
    std::cout << "   Validating against Python semantics\n";
    std::cout << "================================================\n";
    
    test_basic_arithmetic();
    test_let_bindings();
    test_conditionals();
    test_lambda_functions();
    test_complex_programs();
    test_partial_evaluation();
    test_edge_cases();
    
    std::cout << "\n================================================\n";
    std::cout << "   Unit tests completed!\n";
    std::cout << "================================================\n";
    
    return 0;
}