#include "../aperture.hpp"
#include <iostream>
#include <cstdlib>

using namespace secure;

bool test_quote() {
    std::cout << "\n=== Testing quote ===\n";
    
    Evaluator eval;
    
    // Test 1: Basic quote
    {
        Parser p("(quote (+ 1 2))");
        auto expr = p.parse();
        if (!expr) {
            std::cerr << "✗ Parse failed\n";
            return false;
        }
        
        auto result = eval.eval(*expr);
        if (!result) {
            std::cerr << "✗ Eval failed\n";
            return false;
        }
        
        // Result should be unevaluated list (+ 1 2)
        auto* lst = std::get_if<Value::List>(&(*result)->data);
        if (!lst || lst->items.size() != 3) {
            std::cerr << "✗ Quote didn't preserve structure\n";
            return false;
        }
        std::cout << "✓ Basic quote works\n";
    }
    
    // Test 2: Quote syntactic sugar
    {
        Parser p("'(a b c)");
        auto expr = p.parse();
        if (!expr) {
            std::cerr << "✗ Quote sugar parse failed\n";
            return false;
        }
        
        auto result = eval.eval(*expr);
        if (!result) {
            std::cerr << "✗ Quote sugar eval failed\n";
            return false;
        }
        
        auto* lst = std::get_if<Value::List>(&(*result)->data);
        if (!lst || lst->items.size() != 3) {
            std::cerr << "✗ Quote sugar didn't work\n";
            return false;
        }
        std::cout << "✓ Quote syntactic sugar works\n";
    }
    
    return true;
}

bool test_define() {
    std::cout << "\n=== Testing define ===\n";
    
    Evaluator eval;
    
    // Test 1: Simple variable definition
    {
        Parser p1("(define x 42)");
        auto expr1 = p1.parse();
        auto result1 = eval.eval(*expr1);
        
        if (!result1) {
            std::cerr << "✗ Define failed\n";
            return false;
        }
        
        // Now use the defined variable
        Parser p2("x");
        auto expr2 = p2.parse();
        auto result2 = eval.eval(*expr2);
        
        if (!result2) {
            std::cerr << "✗ Variable lookup failed\n";
            return false;
        }
        
        auto* n = std::get_if<Value::Num>(&(*result2)->data);
        if (!n || n->val != 42) {
            std::cerr << "✗ Wrong value\n";
            return false;
        }
        std::cout << "✓ Simple define works\n";
    }
    
    // Test 2: Function definition
    {
        Parser p1("(define (square x) (* x x))");
        auto expr1 = p1.parse();
        auto result1 = eval.eval(*expr1);
        
        if (!result1) {
            std::cerr << "✗ Function define failed\n";
            return false;
        }
        
        // Call the defined function
        Parser p2("(square 5)");
        auto expr2 = p2.parse();
        auto result2 = eval.eval(*expr2);
        
        if (!result2) {
            std::cerr << "✗ Function call failed\n";
            return false;
        }
        
        auto* n = std::get_if<Value::Num>(&(*result2)->data);
        if (!n || n->val != 25) {
            std::cerr << "✗ Wrong function result\n";
            return false;
        }
        std::cout << "✓ Function define works\n";
    }
    
    return true;
}

bool test_cond() {
    std::cout << "\n=== Testing cond ===\n";
    
    Evaluator eval;
    
    // Test multi-way conditional
    Parser p("(cond ((< 5 3) 'first) ((> 5 3) 'second) (else 'third))");
    auto expr = p.parse();
    if (!expr) {
        std::cerr << "✗ Cond parse failed\n";
        return false;
    }
    
    auto result = eval.eval(*expr);
    if (!result) {
        std::cerr << "✗ Cond eval failed\n";
        return false;
    }
    
    // Should select second branch
    auto* sym_result = std::get_if<Value::Sym>(&(*result)->data);
    if (!sym_result || sym_result->name != "second") {
        std::cerr << "✗ Cond selected wrong branch\n";
        return false;
    }
    
    std::cout << "✓ Cond works\n";
    return true;
}

bool test_begin() {
    std::cout << "\n=== Testing begin ===\n";
    
    Evaluator eval;
    
    // Define a variable and use it in begin
    Parser p1("(define counter 0)");
    eval.eval(*p1.parse());
    
    // Begin should return last expression
    Parser p2("(begin (define counter 1) (define counter 2) counter)");
    auto expr = p2.parse();
    if (!expr) {
        std::cerr << "✗ Begin parse failed\n";
        return false;
    }
    
    auto result = eval.eval(*expr);
    if (!result) {
        std::cerr << "✗ Begin eval failed\n";
        return false;
    }
    
    auto* n = std::get_if<Value::Num>(&(*result)->data);
    if (!n || n->val != 2) {
        std::cerr << "✗ Begin didn't return last value\n";
        return false;
    }
    
    std::cout << "✓ Begin works\n";
    return true;
}

bool test_and_or() {
    std::cout << "\n=== Testing and/or ===\n";
    
    Evaluator eval;
    
    // Test AND with short-circuit
    {
        Parser p("(and 1 2 3)");
        auto result = eval.eval(*p.parse());
        
        auto* n = std::get_if<Value::Num>(&(*result)->data);
        if (!n || n->val != 3) {
            std::cerr << "✗ AND didn't return last value\n";
            return false;
        }
        std::cout << "✓ AND returns last value\n";
    }
    
    // Test AND with false
    {
        Parser p("(and 1 0 3)");
        auto result = eval.eval(*p.parse());
        
        auto* n = std::get_if<Value::Num>(&(*result)->data);
        if (!n || n->val != 0) {
            std::cerr << "✗ AND didn't short-circuit\n";
            return false;
        }
        std::cout << "✓ AND short-circuits\n";
    }
    
    // Test OR
    {
        Parser p("(or 0 nil 5 10)");
        auto result = eval.eval(*p.parse());
        
        auto* n = std::get_if<Value::Num>(&(*result)->data);
        if (!n || n->val != 5) {
            std::cerr << "✗ OR didn't return first true\n";
            return false;
        }
        std::cout << "✓ OR returns first true\n";
    }
    
    return true;
}

bool test_list_operations() {
    std::cout << "\n=== Testing list operations ===\n";
    
    Evaluator eval;
    
    // Test cons
    {
        Parser p("(cons 1 '(2 3))");
        auto result = eval.eval(*p.parse());
        if (!result) {
            std::cerr << "✗ Cons failed\n";
            return false;
        }
        
        auto* lst = std::get_if<Value::List>(&(*result)->data);
        if (!lst || lst->items.size() != 3) {
            std::cerr << "✗ Cons wrong size\n";
            return false;
        }
        std::cout << "✓ Cons works\n";
    }
    
    // Test car
    {
        Parser p("(car '(1 2 3))");
        auto result = eval.eval(*p.parse());
        
        auto* n = std::get_if<Value::Num>(&(*result)->data);
        if (!n || n->val != 1) {
            std::cerr << "✗ Car failed\n";
            return false;
        }
        std::cout << "✓ Car works\n";
    }
    
    // Test cdr
    {
        Parser p("(cdr '(1 2 3))");
        auto result = eval.eval(*p.parse());
        
        auto* lst = std::get_if<Value::List>(&(*result)->data);
        if (!lst || lst->items.size() != 2) {
            std::cerr << "✗ Cdr failed\n";
            return false;
        }
        std::cout << "✓ Cdr works\n";
    }
    
    // Test list
    {
        Parser p("(list 1 2 3)");
        auto result = eval.eval(*p.parse());
        
        auto* lst = std::get_if<Value::List>(&(*result)->data);
        if (!lst || lst->items.size() != 3) {
            std::cerr << "✗ List failed\n";
            return false;
        }
        std::cout << "✓ List works\n";
    }
    
    return true;
}

bool test_comparison() {
    std::cout << "\n=== Testing comparison operations ===\n";
    
    Evaluator eval;
    
    // Test =
    {
        Parser p1("(= 5 5)");
        auto result1 = eval.eval(*p1.parse());
        auto* n1 = std::get_if<Value::Num>(&(*result1)->data);
        if (!n1 || n1->val != 1) {
            std::cerr << "✗ = true case failed\n";
            return false;
        }
        
        Parser p2("(= 5 3)");
        auto result2 = eval.eval(*p2.parse());
        auto* n2 = std::get_if<Value::Num>(&(*result2)->data);
        if (!n2 || n2->val != 0) {
            std::cerr << "✗ = false case failed\n";
            return false;
        }
        std::cout << "✓ = works\n";
    }
    
    // Test <
    {
        Parser p1("(< 3 5)");
        auto result1 = eval.eval(*p1.parse());
        auto* n1 = std::get_if<Value::Num>(&(*result1)->data);
        if (!n1 || n1->val != 1) {
            std::cerr << "✗ < true case failed\n";
            return false;
        }
        std::cout << "✓ < works\n";
    }
    
    // Test >
    {
        Parser p1("(> 5 3)");
        auto result1 = eval.eval(*p1.parse());
        auto* n1 = std::get_if<Value::Num>(&(*result1)->data);
        if (!n1 || n1->val != 1) {
            std::cerr << "✗ > true case failed\n";
            return false;
        }
        std::cout << "✓ > works\n";
    }
    
    return true;
}

int main() {
    std::cout << "================================================\n";
    std::cout << "   SPECIAL FORMS TEST SUITE\n";
    std::cout << "================================================\n";
    
    int passed = 0;
    int failed = 0;
    
    if (test_quote()) passed++; else failed++;
    if (test_define()) passed++; else failed++;
    if (test_cond()) passed++; else failed++;
    if (test_begin()) passed++; else failed++;
    if (test_and_or()) passed++; else failed++;
    if (test_list_operations()) passed++; else failed++;
    if (test_comparison()) passed++; else failed++;
    
    std::cout << "\n================================================\n";
    std::cout << "Results:\n";
    std::cout << "Passed: " << passed << "/" << (passed + failed) << "\n";
    std::cout << "Failed: " << failed << "/" << (passed + failed) << "\n";
    std::cout << "================================================\n";
    
    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}