#include "fluent_api.hpp"
#include <iostream>

using namespace aperture;
using namespace aperture::build;

int main() {
    std::cout << "=== Simple Fluent API Demo ===\n\n";
    
    // Test 1: Basic arithmetic
    std::cout << "1. Basic arithmetic:\n";
    Expr basic = num(10) + num(20);
    std::cout << "   Expression: " << basic << "\n";
    Computation comp1(basic);
    std::cout << "   Result: " << comp1.eval() << "\n\n";
    
    // Test 2: Expression with holes
    std::cout << "2. Expression with holes:\n";
    Expr with_hole = hole("x") + num(10);
    std::cout << "   Expression: " << with_hole << "\n";
    
    // Fill hole and evaluate
    Computation comp2(with_hole);
    comp2.fill("x", 5);
    std::cout << "   With x=5: " << comp2.eval() << "\n\n";
    
    // Test 3: Complex expression
    std::cout << "3. Complex expression:\n";
    Expr complex = (hole("x") + num(2)) * num(3);
    std::cout << "   Expression: " << complex << "\n";
    
    Computation comp3(complex);
    comp3.fill("x", 4);
    std::cout << "   With x=4: (4+2)*3 = " << comp3.eval() << "\n\n";
    
    // Test 4: Using builder functions
    std::cout << "4. Using builder shortcuts:\n";
    Expr built = x() * num(2) + y();
    std::cout << "   Expression: " << built << "\n";
    
    Computation comp4(built);
    comp4.fill({{"x", 10}, {"y", 5}});
    std::cout << "   With x=10, y=5: " << comp4.eval() << "\n\n";
    
    // Test 5: Let binding
    std::cout << "5. Let binding:\n";
    Expr let_expr = ops::let("a", 10, Expr("a") + num(5));
    std::cout << "   Expression: " << let_expr << "\n";
    Computation comp5(let_expr);
    std::cout << "   Result: " << comp5.eval() << "\n\n";
    
    // Test 6: Conditional
    std::cout << "6. Conditional:\n";
    Expr cond = ops::if_then_else(
        hole("x") > num(5),
        hole("x") * num(2),
        hole("x") + num(10)
    );
    std::cout << "   Expression: " << cond << "\n";
    
    Computation comp6a(cond);
    comp6a.fill("x", 8);
    std::cout << "   With x=8 (>5): " << comp6a.eval() << "\n";
    
    Computation comp6b(cond);
    comp6b.fill("x", 3);
    std::cout << "   With x=3 (<=5): " << comp6b.eval() << "\n\n";
    
    // Test 7: Lambda
    std::cout << "7. Lambda function:\n";
    Expr lambda_expr = ops::lambda("x", [](Expr x) { 
        return x * num(2) + num(1); 
    });
    std::cout << "   Lambda: " << lambda_expr << "\n";
    
    Expr applied = lambda_expr(num(10));
    std::cout << "   Applied to 10: " << applied << "\n";
    Computation comp7(applied);
    std::cout << "   Result: " << comp7.eval() << "\n\n";
    
    // Test 8: Chaining operations
    std::cout << "8. Chaining operations:\n";
    auto result = Computation((hole("x") + num(3)) * num(2))
        .fill("x", 7)
        .eval();
    std::cout << "   (x+3)*2 with x=7: " << result << "\n\n";
    
    std::cout << "=== All tests completed successfully! ===\n";
    
    return 0;
}