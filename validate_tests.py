#!/usr/bin/env python3
"""
Validation script to verify Aperture language test cases against Python.
This ensures our test expectations match Python's behavior.
"""

import math

def run_test(name, python_code, expected):
    """Run a test and verify the result matches expected value."""
    try:
        result = eval(python_code)
        passed = abs(result - expected) < 1e-9
        status = "✓" if passed else "✗"
        print(f"{status} {name}: {python_code} = {result} (expected {expected})")
        if not passed:
            print(f"  Difference: {abs(result - expected)}")
        return passed
    except Exception as e:
        print(f"✗ {name}: {python_code} raised {e}")
        return False

def test_arithmetic():
    """Test arithmetic operations."""
    print("\n=== Arithmetic Tests ===")
    tests = [
        ("Addition", "1 + 2", 3.0),
        ("Addition multiple", "1 + 2 + 3 + 4", 10.0),
        ("Subtraction", "5 - 3", 2.0),
        ("Multiplication", "3 * 4", 12.0),
        ("Division", "10 / 2", 5.0),
        ("Nested 1", "(2 + 3) * 4", 20.0),
        ("Nested 2", "10 / (2 + 3)", 2.0),
        ("Complex nested", "((3 * 4) + (5 * 6)) / 2", 21.0),
        ("Float addition", "1.5 + 2.5", 4.0),
        ("Float division", "7.5 / 2.5", 3.0),
        ("Add zero", "0 + 5", 5.0),
        ("Multiply by zero", "100 * 0", 0.0),
        ("Negative numbers", "-5 + 3", -2.0),
        ("Negative multiplication", "(-3) * (-4)", 12.0),
        ("Order 1", "2 + (3 * 4)", 14.0),
        ("Large number", "1e10 + 1e10", 2e10),
        ("Small number", "1e-10 * 2", 2e-10),
    ]
    
    passed = sum(run_test(*test) for test in tests)
    print(f"\nPassed: {passed}/{len(tests)}")
    return passed == len(tests)

def test_let_bindings():
    """Test let bindings (simulated with Python variables)."""
    print("\n=== Let Binding Tests ===")
    tests = []
    
    # Simple let
    x = 10
    tests.append(("Simple let", f"{x} + 5", 15.0))
    
    # Multiple bindings
    x = 2
    y = 3
    tests.append(("Multiple bindings", f"{x} * {y}", 6.0))
    
    # Nested let
    x = 5
    y = x + 2
    tests.append(("Nested let", f"{y} * 2", 14.0))
    
    # Sequential binding
    a = 10
    b = a * 2
    tests.append(("Sequential binding", f"{a} + {b}", 30.0))
    
    # Shadow binding
    x = 10
    x = 5  # Inner binding shadows outer
    tests.append(("Shadow binding", f"{x}", 5.0))
    
    # Complex expression in binding
    x = (2 + 3) * 4
    tests.append(("Complex binding", f"{x} / 2", 10.0))
    
    passed = sum(run_test(*test) for test in tests)
    print(f"\nPassed: {passed}/{len(tests)}")
    return passed == len(tests)

def test_conditionals():
    """Test conditional expressions."""
    print("\n=== Conditional Tests ===")
    tests = [
        ("If true", "5 if 1 else 10", 5.0),
        ("If false", "5 if 0 else 10", 10.0),
        ("If nil", "5 if None else 10", 10.0),
        ("If negative", "5 if -1 else 10", 5.0),
        ("If computed", "5 if (3 - 2) else 10", 5.0),
        ("Nested if", "(5 if 1 else 10) if 1 else 20", 5.0),
        ("If complex", "(2 * 3) if (5 - 5) else (4 + 6)", 10.0),
    ]
    
    # If in let binding
    x = 5 if 1 else 10
    tests.append(("If in let", f"{x} * 2", 10.0))
    
    passed = sum(run_test(*test) for test in tests)
    print(f"\nPassed: {passed}/{len(tests)}")
    return passed == len(tests)

def test_lambda_functions():
    """Test lambda functions."""
    print("\n=== Lambda Function Tests ===")
    tests = [
        ("Lambda identity", "(lambda x: x)(10)", 10.0),
        ("Lambda add", "(lambda x: x + 5)(10)", 15.0),
        ("Lambda multi-param", "(lambda x, y: x * y)(3, 4)", 12.0),
        ("Lambda three params", "(lambda x, y, z: x + y + z)(1, 2, 3)", 6.0),
        ("Nested lambda", "(lambda x: (lambda y: x + y))(10)(5)", 15.0),
        ("Complex lambda body", "(lambda x: (x + 1) * (x - 1))(5)", 24.0),
    ]
    
    # Closure over variable
    x = 10
    f = lambda y: x + y
    tests.append(("Lambda closure", f"(lambda y: {x} + y)(5)", 15.0))
    
    # Lambda in variable
    f = lambda x: x * 2
    tests.append(("Lambda in let", f"(lambda x: x * 2)(7)", 14.0))
    
    # Higher-order function
    apply = lambda f, x: f(x)
    tests.append(("Higher order", f"(lambda f, x: f(x))(lambda y: y + 1, 9)", 10.0))
    
    passed = sum(run_test(*test) for test in tests)
    print(f"\nPassed: {passed}/{len(tests)}")
    return passed == len(tests)

def test_complex_programs():
    """Test complex program equivalents."""
    print("\n=== Complex Program Tests ===")
    tests = [
        ("Factorial 5", "1 * 2 * 3 * 4 * 5", 120.0),
        ("Quadratic discriminant", 
         "(lambda a, b, c: b*b - 4*a*c)(1, -5, 6)", 1.0),
        ("Distance squared",
         "(lambda x1, y1, x2, y2: (x2-x1)**2 + (y2-y1)**2)(0, 0, 3, 4)", 25.0),
        ("Polynomial eval",
         "(lambda x: 2*x**3 + 3*x**2 - 4*x + 5)(2)", 25.0),
        ("Compound interest",
         "(lambda p, r: p * (1+r)**3)(1000, 0.05)", 1157.625),
    ]
    
    # Fibonacci 6th element
    f0, f1 = 1, 1
    f2 = f0 + f1
    f3 = f1 + f2
    f4 = f2 + f3
    f5 = f3 + f4
    tests.append(("Fibonacci 6th", str(f5), 8.0))
    
    passed = sum(run_test(*test) for test in tests)
    print(f"\nPassed: {passed}/{len(tests)}")
    return passed == len(tests)

def test_edge_cases():
    """Test edge cases."""
    print("\n=== Edge Cases ===")
    tests = [
        ("Single number", "42", 42.0),
        ("Deep nesting", "1 + (2 + (3 + (4 + (5 + (6 + 7)))))", 28.0),
        ("Many args", "sum([1, 2, 3, 4, 5, 6, 7, 8, 9, 10])", 55.0),
    ]
    
    passed = sum(run_test(*test) for test in tests)
    print(f"\nPassed: {passed}/{len(tests)}")
    return passed == len(tests)

def main():
    """Run all test validations."""
    print("=" * 50)
    print("   PYTHON VALIDATION OF APERTURE TEST CASES")
    print("=" * 50)
    
    all_passed = True
    all_passed &= test_arithmetic()
    all_passed &= test_let_bindings()
    all_passed &= test_conditionals()
    all_passed &= test_lambda_functions()
    all_passed &= test_complex_programs()
    all_passed &= test_edge_cases()
    
    print("\n" + "=" * 50)
    if all_passed:
        print("✓ All Python validations passed!")
        print("Test expectations are correctly aligned with Python semantics.")
    else:
        print("✗ Some validations failed.")
        print("Test expectations may need adjustment.")
    print("=" * 50)

if __name__ == "__main__":
    main()