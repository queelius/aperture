# Getting Started with Aperture

This guide will walk you through your first Aperture programs and introduce the key concepts.

## Your First Program

Let's start with a simple arithmetic expression:

```lisp
(+ 1 2 3)
```

This evaluates to `6`. Aperture uses prefix notation (also called Polish notation) where the operator comes first.

## Using the C++ API

```cpp
#include "aperture.hpp"
#include <iostream>

using namespace secure;

int main() {
    // Parse an expression
    Parser parser("(+ 1 2 3)");
    auto expr = parser.parse();
    
    if (!expr) {
        std::cerr << "Parse error: " << expr.error().message << "\n";
        return 1;
    }
    
    // Evaluate it
    Evaluator eval;
    auto result = eval.eval(*expr);
    
    if (!result) {
        std::cerr << "Eval error: " << result.error().message << "\n";
        return 1;
    }
    
    // Print result
    std::cout << "Result: " << to_string(*result) << "\n";
    return 0;
}
```

## Working with Holes

The key innovation of Aperture is holes - placeholders for values that will be provided later:

```lisp
(+ 10 ?x 5)
```

This expression has a hole `?x`. When partially evaluated, it simplifies to:

```lisp
(+ 15 ?x)
```

### Complete Example with Holes

```cpp
#include "aperture.hpp"
#include <iostream>
#include <unordered_map>

using namespace secure;

int main() {
    // Expression with a hole
    Parser parser("(+ (* ?x 2) 10)");
    auto expr = parser.parse();
    
    Evaluator eval;
    
    // Partial evaluation - simplifies while preserving holes
    auto partial = eval.partial_eval(*expr);
    std::cout << "Partial: " << to_string(*partial) << "\n";
    
    // Fill the hole
    std::unordered_map<std::string, ValuePtr> values;
    values["x"] = num(5);
    
    auto filled = eval.fill_holes(*partial, values);
    
    // Final evaluation
    auto result = eval.eval(*filled);
    
    if (auto* n = std::get_if<Value::Num>(&(*result)->data)) {
        std::cout << "Result: " << n->val << "\n";  // Output: 20
    }
    
    return 0;
}
```

## Defining Functions

Functions in Aperture are created with `lambda`:

```lisp
(lambda (x) (* x x))  ; Square function
```

Apply a function by calling it:

```lisp
((lambda (x) (* x x)) 5)  ; Returns 25
```

### Functions with Multiple Parameters

```lisp
((lambda (x y) (+ x y)) 10 20)  ; Returns 30
```

## Local Bindings with Let

The `let` form creates local bindings:

```lisp
(let ((x 10) (y 20))
  (+ x y))  ; Returns 30
```

Bindings can reference each other:

```lisp
(let ((x 5)
      (y (* x 2)))
  y)  ; Returns 10
```

## Conditionals

Use `if` for conditional evaluation:

```lisp
(if (> x 0)
    "positive"
    "non-positive")
```

The condition is evaluated first. Any non-nil, non-zero value is considered true.

## Secure Computation Example

Here's a complete example showing secure distributed computation:

```cpp
#include "aperture.hpp"
#include <iostream>

using namespace secure;

// Simulated untrusted server
class Server {
public:
    ValuePtr optimize(ValuePtr expr) {
        Evaluator eval;
        auto result = eval.partial_eval(expr);
        return result ? *result : nil();
    }
};

// Trusted client
class Client {
    std::unordered_map<std::string, double> private_data;
    
public:
    void set_private(const std::string& name, double val) {
        private_data[name] = val;
    }
    
    double compute(const std::string& formula) {
        // Parse formula
        Parser parser(formula);
        auto expr = parser.parse();
        if (!expr) return 0;
        
        // Send to server for optimization
        Server server;
        auto optimized = server.optimize(*expr);
        
        // Fill holes locally
        Evaluator eval;
        std::unordered_map<std::string, ValuePtr> values;
        for (const auto& [key, val] : private_data) {
            values[key] = num(val);
        }
        
        auto filled = eval.fill_holes(optimized, values);
        if (!filled) return 0;
        
        // Evaluate locally
        auto result = eval.eval(*filled);
        if (!result) return 0;
        
        if (auto* n = std::get_if<Value::Num>(&(*result)->data)) {
            return n->val;
        }
        return 0;
    }
};

int main() {
    Client client;
    client.set_private("income", 75000);
    client.set_private("deductions", 12000);
    
    // Tax calculation formula (public)
    std::string formula = 
        "(let ((rate 0.22)) "
        "  (- (* ?income rate) ?deductions))";
    
    double tax = client.compute(formula);
    std::cout << "Tax owed: $" << tax << "\n";
    
    return 0;
}
```

## Using the Fluent API

The fluent API provides a more natural C++ syntax:

```cpp
#include "fluent_api.hpp"
using namespace aperture;
using namespace aperture::build;

int main() {
    // Build expression with operators
    auto expr = (hole("x") + num(10)) * num(2);
    
    // Evaluate with hole filling
    Computation comp(expr);
    double result = comp.fill("x", 5).eval();
    
    std::cout << "Result: " << result << "\n";  // 30
    
    return 0;
}
```

## Next Steps

- Read the [Language Design Document](DESIGN.md) for complete language specification
- Explore [Examples](examples.md) for more use cases
- Learn about the [Security Model](security.md)
- Check the [API Reference](api/html/index.html) for detailed documentation

## Common Patterns

### Pattern 1: Secure Aggregation

```lisp
; Public formula for average
(/ (+ ?val1 ?val2 ?val3) 3)

; Each party fills their value locally
```

### Pattern 2: Conditional Logic with Private Data

```lisp
(if (> ?age 65)
    (* ?base_amount 0.8)  ; Senior discount
    ?base_amount)
```

### Pattern 3: Symbolic Computation

```lisp
; Quadratic formula with symbolic coefficients
(let ((a ?a) (b ?b) (c ?c))
  (- (* b b) (* 4 (* a c))))  ; Discriminant
```

## Troubleshooting

### Parse Errors

- Check parentheses are balanced
- Ensure strings are properly quoted
- Verify hole syntax: `?name` or `?{name:constraint}`

### Evaluation Errors

- Undefined symbols: Check spelling and scope
- Type errors: Ensure operations match types
- Division by zero: Add appropriate checks

### Performance Issues

- Deep nesting: Consider restructuring expression
- Many holes: Partial evaluation may be complex
- Large expressions: Break into smaller parts