# Aperture Language Examples

This page contains practical examples demonstrating various features and use cases of the Aperture language.

## Basic Examples

### Arithmetic Operations

```lisp
; Simple addition
(+ 1 2 3)  ; => 6

; Nested operations  
(* (+ 2 3) 4)  ; => 20

; Mixed operations
(/ (+ 10 20) (- 7 2))  ; => 6
```

### Variables and Bindings

```lisp
; Let bindings
(let ((x 10) (y 20))
  (+ x y))  ; => 30

; Nested let
(let ((x 5))
  (let ((y (* x 2)))
    (+ x y)))  ; => 15

; Sequential dependencies
(let ((base 100)
      (tax (* base 0.1))
      (total (+ base tax)))
  total)  ; => 110
```

### Functions (Lambdas)

```lisp
; Identity function
((lambda (x) x) 42)  ; => 42

; Square function
((lambda (x) (* x x)) 5)  ; => 25

; Multiple parameters
((lambda (x y z) (+ x y z)) 10 20 30)  ; => 60

; Higher-order function
(let ((apply (lambda (f x) (f x))))
  (apply (lambda (y) (* y 2)) 21))  ; => 42
```

## Working with Holes

### Simple Holes

```lisp
; Basic hole
?x

; Expression with hole
(+ 10 ?x)

; Multiple holes
(* ?rate ?principal)
```

### Partial Evaluation Examples

```lisp
; Original expression
(+ 10 ?x 20 ?y)
; After partial eval: (+ 30 ?x ?y)

; With multiplication
(* 2 ?x 3 ?y)
; After partial eval: (* 6 ?x ?y)

; Zero elimination
(* 0 ?anything)
; After partial eval: 0

; Identity elimination  
(+ 0 ?x)
; After partial eval: ?x
```

## Real-World Use Cases

### 1. Tax Calculation (Finance)

```lisp
(let ((gross_income ?income)
      (standard_deduction 12950)
      (tax_brackets '((10 10275)
                      (12 41775)
                      (22 89075))))
  (let ((taxable_income (- gross_income standard_deduction)))
    (if (< taxable_income 0)
        0
        (* taxable_income 0.22))))  ; Simplified
```

Usage in C++:
```cpp
Parser parser(tax_formula);
auto expr = parser.parse();

// Server optimizes without seeing income
auto optimized = server.partial_eval(*expr);

// Client fills income locally
values["income"] = num(75000);
auto result = eval.fill_holes(*optimized, values);
```

### 2. Medical Dosage Calculation

```lisp
(let ((base_dose 10)
      (weight_factor 1.5)
      (age_adjustment (if (> ?age 65) 0.8 1.0)))
  (* base_dose
     (/ ?weight 70)
     weight_factor
     age_adjustment
     (+ 0.5 (* ?severity 0.5))))
```

This allows:
- Hospital defines formula (public)
- Patient data remains private
- Cloud service optimizes formula
- Final calculation happens locally

### 3. Portfolio Risk Assessment

```lisp
(let ((volatilities '((BTC 0.8) (ETH 0.9) (STOCKS 0.3)))
      (correlations '(...)))
  (+ (* ?btc_position 0.8 0.8)
     (* ?eth_position 0.9 0.9)
     (* ?stock_position 0.3 0.3)))  ; Simplified
```

### 4. Machine Learning Inference

```lisp
; Logistic regression model
(let ((weights '(0.5 -0.3 0.8))
      (bias -1.2))
  (if (> (+ (* 0.5 ?feature1)
            (* -0.3 ?feature2)
            (* 0.8 ?feature3)
            bias)
         0)
      "approved"
      "denied"))
```

### 5. A/B Testing with Privacy

```lisp
; Conversion rate calculation
(let ((control_rate 0.05)
      (treatment_effect 1.2))
  (if ?is_treatment
      (* ?base_conversion control_rate treatment_effect)
      (* ?base_conversion control_rate)))
```

## Advanced Patterns

### Pattern 1: Recursive Structures

```lisp
; Factorial (unrolled)
(let ((factorial_5 (* 5 (* 4 (* 3 (* 2 1))))))
  factorial_5)  ; => 120

; Fibonacci (unrolled)
(let ((fib (lambda (n)
            (if (<= n 1)
                n
                (+ (fib (- n 1))
                   (fib (- n 2)))))))
  (fib 5))  ; Would need memoization
```

### Pattern 2: Symbolic Computation

```lisp
; Quadratic formula
(let ((a ?a) (b ?b) (c ?c))
  (let ((discriminant (- (* b b) (* 4 a c))))
    (if (< discriminant 0)
        "no real roots"
        (/ (+ (- b) discriminant) (* 2 a)))))
```

### Pattern 3: Constraint Systems

```lisp
; Linear constraints
(let ((constraints (list (< (+ ?x ?y) 100)
                        (> ?x 0)
                        (> ?y 0)
                        (= (* 2 ?x) ?y))))
  constraints)
```

### Pattern 4: Template Metaprogramming

```lisp
; Code generation template
(let ((make_getter (lambda (field)
                     (lambda (obj)
                       (get obj field)))))
  (let ((get_name (make_getter 'name))
        (get_age (make_getter 'age)))
    (list (get_name ?person)
          (get_age ?person))))
```

## Security Examples

### Secure Multi-Party Computation

```lisp
; Average salary without revealing individual salaries
(/ (+ ?salary1 ?salary2 ?salary3) 3)

; Each party evaluates locally:
; Party 1: fills salary1, keeps others as holes
; Party 2: fills salary2, keeps others as holes
; Party 3: fills salary3, keeps others as holes
; Combine results securely
```

### Zero-Knowledge Proof Pattern

```lisp
; Prove age > 21 without revealing actual age
(let ((age_threshold 21))
  (if (> ?age age_threshold)
      "valid"
      "invalid"))
```

### Differential Privacy

```lisp
; Add noise for privacy
(let ((true_value ?sensitive_data)
      (noise (random -10 10)))  ; Laplace noise
  (+ true_value noise))
```

## Performance Optimization Examples

### Constant Folding

```lisp
; Before optimization
(+ 1 2 3 ?x 4 5)

; After partial eval
(+ 15 ?x)
```

### Common Subexpression Elimination

```lisp
; Before
(let ((x (+ ?a ?b))
      (y (+ ?a ?b)))
  (* x y))

; Optimized
(let ((temp (+ ?a ?b)))
  (* temp temp))
```

### Algebraic Simplification

```lisp
; Examples of automatic simplifications:
(* 1 ?x)        ; => ?x
(* 0 ?x)        ; => 0
(+ 0 ?x)        ; => ?x
(- ?x 0)        ; => ?x
(/ ?x 1)        ; => ?x
(* ?x (/ 1 ?x)) ; => 1 (if x ≠ 0)
```

## Integration Examples

### With Python (via subprocess)

```python
import subprocess
import json

def evaluate_aperture(expr, holes=None):
    """Evaluate Aperture expression with holes"""
    cmd = ["./aperture_eval", "--expr", expr]
    if holes:
        cmd.extend(["--holes", json.dumps(holes)])
    
    result = subprocess.run(cmd, capture_output=True, text=True)
    return json.loads(result.stdout)

# Use it
result = evaluate_aperture(
    "(+ (* ?x 2) 10)",
    {"x": 5}
)
print(result)  # 20
```

### With Node.js (via FFI)

```javascript
const ffi = require('ffi-napi');

const aperture = ffi.Library('./libaperture', {
  'eval_expr': ['double', ['string', 'string']]
});

function evaluate(expr, holes = {}) {
  const holesJson = JSON.stringify(holes);
  return aperture.eval_expr(expr, holesJson);
}

// Use it
const result = evaluate('(+ ?x 10)', {x: 5});
console.log(result); // 15
```

### REST API Server

```cpp
// Using cpp-httplib
#include "httplib.h"
#include "aperture.hpp"

int main() {
    httplib::Server srv;
    
    srv.Post("/eval", [](const httplib::Request& req, httplib::Response& res) {
        auto expr_str = req.get_param_value("expr");
        auto holes_json = req.get_param_value("holes");
        
        Parser parser(expr_str);
        auto expr = parser.parse();
        
        Evaluator eval;
        auto result = eval.partial_eval(*expr);
        
        res.set_content(to_string(*result), "text/plain");
    });
    
    srv.listen("localhost", 8080);
}
```

## Error Handling Examples

### Graceful Degradation

```lisp
(let ((safe_divide (lambda (a b)
                     (if (= b 0)
                         'infinity
                         (/ a b)))))
  (safe_divide ?numerator ?denominator))
```

### Type Checking

```lisp
(let ((safe_add (lambda (a b)
                  (if (and (number? a) (number? b))
                      (+ a b)
                      'type_error))))
  (safe_add ?x ?y))
```

### Validation

```lisp
(let ((validate_age (lambda (age)
                      (and (number? age)
                           (>= age 0)
                           (<= age 150)))))
  (if (validate_age ?age)
      (process ?age)
      'invalid_age))
```