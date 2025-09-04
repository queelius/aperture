# Aperture Language Design Document

## Table of Contents
1. [Overview](#overview)
2. [Language Fundamentals](#language-fundamentals)
3. [Special Forms](#special-forms)
4. [Built-in Functions (Prelude)](#built-in-functions-prelude)
5. [Apertures and Holes](#apertures-and-holes)
6. [Fluent C++ API](#fluent-c-api)
7. [Security Model](#security-model)
8. [Implementation Details](#implementation-details)

## Overview

Aperture is a security-focused programming language that formalizes ambiguity through "apertures" - partial program specifications with holes that can be filled later. The language enables secure distributed computation where untrusted servers can optimize expressions without accessing sensitive data.

### Key Principles

1. **Privacy by Design**: Sensitive data never leaves the trusted environment
2. **Partial Evaluation**: Expressions simplify as much as possible while preserving holes
3. **Algebraic Simplification**: Mathematical expressions reduce to simpler forms
4. **Type Safety**: Runtime type checking with clear error messages
5. **Functional Purity**: Immutable values and referential transparency

## Language Fundamentals

### Syntax

Aperture uses S-expression syntax inspired by Scheme/Lisp:

```lisp
; Comments start with semicolon
(operator arg1 arg2 ...)  ; Function application
?variable                  ; Hole (placeholder for private data)
?{var:constraint}          ; Hole with type/range constraint
123                        ; Numbers (integers and floats)
"hello"                    ; Strings
symbol                     ; Symbols/identifiers
```

### Value Types

| Type | Description | Example |
|------|-------------|---------|
| `Nil` | Empty/null value | `nil` |
| `Num` | Double-precision number | `42`, `3.14`, `1e-10` |
| `Str` | UTF-8 string | `"hello world"` |
| `Sym` | Symbol/identifier | `x`, `+`, `lambda` |
| `List` | S-expression list | `(1 2 3)` |
| `Lambda` | Function closure | `(lambda (x) (* x 2))` |
| `Hole` | Private data placeholder | `?income`, `?{age:int}` |

### Evaluation Model

1. **Eager Evaluation**: Arguments evaluated before function application
2. **Partial Evaluation**: Expressions with holes reduce as much as possible
3. **Environment Binding**: Lexical scoping with closures
4. **Tail Call Optimization**: (planned) Recursive functions in tail position

## Special Forms

Special forms have custom evaluation rules and cannot be implemented as functions:

### `if` - Conditional Expression

```lisp
(if condition then-expr else-expr)
```

- Evaluates `condition` first
- If truthy (non-nil, non-zero), evaluates and returns `then-expr`
- Otherwise, evaluates and returns `else-expr`
- With holes: returns simplified if-expression

Examples:
```lisp
(if (> x 0) "positive" "non-positive")
(if ?condition 100 200)  ; Preserves hole in condition
```

### `let` - Local Bindings

```lisp
(let ((var1 val1) (var2 val2) ...) body)
```

- Creates new lexical scope
- Binds variables to values sequentially
- Evaluates body in extended environment
- Variables shadow outer bindings

Examples:
```lisp
(let ((x 10) (y 20)) (+ x y))           ; => 30
(let ((x 5)) (let ((x 10)) x))          ; => 10 (shadowing)
(let ((rate 0.1)) (* ?principal rate))  ; Partial eval with hole
```

### `lambda` - Function Definition

```lisp
(lambda (param1 param2 ...) body)
```

- Creates closure capturing current environment
- Parameters bound when function is applied
- Body evaluated in extended environment
- Supports partial application with holes

Examples:
```lisp
(lambda (x) (* x x))                    ; Square function
(lambda (x y) (+ x y))                  ; Two parameters
((lambda (x) (+ x ?secret)) 10)         ; => (+ 10 ?secret)
```

### `quote` (planned)

```lisp
(quote expr)
'expr  ; Syntactic sugar
```

- Returns expression unevaluated as data
- Useful for symbolic computation

### `define` (planned)

```lisp
(define name value)
(define (name params...) body)  ; Function sugar
```

- Binds value in global environment
- Function form expands to lambda

## Built-in Functions (Prelude)

### Arithmetic Operations

| Function | Arity | Description | Example |
|----------|-------|-------------|---------|
| `+` | 0+ | Addition | `(+ 1 2 3)` => 6 |
| `-` | 2 | Subtraction | `(- 5 3)` => 2 |
| `*` | 0+ | Multiplication | `(* 2 3 4)` => 24 |
| `/` | 2 | Division | `(/ 10 2)` => 5 |

Arithmetic with holes:
```lisp
(+ 10 ?x 5)        ; => (+ 15 ?x)
(* 0 ?anything)    ; => 0 (algebraic simplification)
(* ?x 1)           ; => ?x (identity elimination)
```

### Comparison Operations (planned)

| Function | Description | Example |
|----------|-------------|---------|
| `=` | Numeric equality | `(= x y)` |
| `<` | Less than | `(< x y)` |
| `>` | Greater than | `(> x y)` |
| `<=` | Less or equal | `(<= x y)` |
| `>=` | Greater or equal | `(>= x y)` |

### List Operations (planned)

| Function | Description | Example |
|----------|-------------|---------|
| `cons` | Construct pair | `(cons 1 (2 3))` => `(1 2 3)` |
| `car` | First element | `(car (1 2 3))` => 1 |
| `cdr` | Rest of list | `(cdr (1 2 3))` => `(2 3)` |
| `list` | Create list | `(list 1 2 3)` => `(1 2 3)` |
| `null?` | Test for nil | `(null? nil)` => true |

### Type Predicates (planned)

| Function | Description |
|----------|-------------|
| `number?` | Test if number |
| `string?` | Test if string |
| `symbol?` | Test if symbol |
| `list?` | Test if list |
| `lambda?` | Test if function |
| `hole?` | Test if hole |

## Apertures and Holes

### Concept

An **aperture** is a partial program specification with "holes" - placeholders for values that will be provided later. This enables:

1. **Secure Computation**: Process data without exposing sensitive values
2. **Symbolic Computation**: Work with algebraic expressions
3. **Lazy Evaluation**: Defer computation until values are available
4. **Program Templates**: Reusable patterns with parameters

### Hole Syntax

```lisp
?variable              ; Simple hole
?{var:type}            ; Typed hole (future)
?{var:range(0,100)}    ; Constrained hole (future)
?{var:sealed}          ; Immutable hole (future)
```

### Partial Evaluation

When an expression contains holes, evaluation proceeds as far as possible:

```lisp
(+ 10 ?x 20)           ; => (+ 30 ?x)
(* ?x 0)               ; => 0
(if ?cond A B)         ; => (if ?cond A B) [preserved]
(let ((y 5)) (+ ?x y)) ; => (+ ?x 5)
```

### Hole Operations

```cpp
// Find all holes in an expression
std::vector<std::string> find_holes(ValuePtr expr);

// Fill holes with values
ValuePtr fill_holes(ValuePtr expr, 
                   const std::unordered_map<std::string, ValuePtr>& values);

// Partially evaluate with holes
ValuePtr partial_eval(ValuePtr expr);
```

### Security Workflow

```
1. Client prepares expression with private data as holes
2. Send to untrusted server for optimization
3. Server performs partial evaluation
4. Client verifies returned expression
5. Client fills holes locally
6. Client evaluates final result
```

Example:
```lisp
; Original (at client)
(let ((rate 0.25)) (* ?income rate))

; After server optimization
(* ?income 0.25)

; Client fills and evaluates
income = 50000 => 12500
```

## Fluent C++ API

The fluent API provides a natural C++ interface for building and evaluating Aperture expressions:

### Expression Building

```cpp
using namespace aperture;
using namespace aperture::build;

// Basic expressions
auto expr1 = num(42);
auto expr2 = hole("x");
auto expr3 = expr1 + expr2;  // (+ 42 ?x)

// Complex expressions
auto formula = (hole("income") * num(0.25)) - num(5000);

// Lambda expressions
auto fn = lambda({"x", "y"}, x() + y());
```

### Computation Classes

```cpp
// Basic computation
Computation comp(expr);
double result = comp.fill("x", 10)
                   .fill("y", 20)
                   .eval();

// Secure computation
SecureComputation secure(expr);
auto optimized = secure.send_to_server();  // Partial eval
auto verified = secure.verify_holes();     // Security check
double final = secure.fill({{"x", 10}})
                    .eval();
```

### Builder Functions

```cpp
namespace aperture::build {
    Expr hole(const std::string& name);
    Expr num(double n);
    Expr sym(const std::string& s);
    Expr str(const std::string& s);
    
    // Convenience hole builders
    Expr x();  // hole("x")
    Expr y();  // hole("y")
    Expr z();  // hole("z")
}
```

### Operator Overloading

```cpp
// Arithmetic
expr1 + expr2   // (+ expr1 expr2)
expr1 - expr2   // (- expr1 expr2)
expr1 * expr2   // (* expr1 expr2)
expr1 / expr2   // (/ expr1 expr2)

// Comparison (planned)
expr1 == expr2  // (= expr1 expr2)
expr1 < expr2   // (< expr1 expr2)
expr1 > expr2   // (> expr1 expr2)
```

## Security Model

### Threat Model

1. **Untrusted Computation**: Servers may try to extract private data
2. **Side Channels**: Timing or pattern analysis attacks
3. **Malicious Optimization**: Servers return incorrect results
4. **Complexity Attacks**: DoS through expensive computations

### Security Properties

1. **Data Privacy**: Holes never reveal actual values to untrusted parties
2. **Computation Integrity**: Clients verify expression structure before filling
3. **Complexity Bounds**: Maximum recursion depth and evaluation steps
4. **Taint Tracking**: Values derived from holes are marked as tainted
5. **Sealed Holes**: Immutable holes that cannot be modified

### Security Limits

```cpp
class Evaluator {
    size_t max_depth = 1000;        // Stack overflow prevention
    size_t max_complexity = 1000000; // DoS prevention
    size_t max_memory = 100'000'000; // Memory exhaustion prevention
};
```

## Implementation Details

### Memory Management

- **Smart Pointers**: `std::shared_ptr<Value>` for automatic memory management
- **Value Semantics**: Immutable values, no mutation
- **Copy-on-Write**: (planned) Efficient copying of large structures

### Parser Architecture

- **Recursive Descent**: Simple, predictable parsing
- **String Views**: Zero-copy tokenization
- **Error Recovery**: (planned) Continue parsing after errors

### Evaluator Design

- **Stack-Based**: Call frames for environment management
- **Visitor Pattern**: Type-safe expression traversal
- **Partial Evaluation**: Algebraic simplification rules

### Type System

- **Dynamic Typing**: Runtime type checking
- **Variant Storage**: `std::variant` for type safety
- **Type Inference**: (planned) Deduce types from usage

### Performance Optimizations

1. **Constant Folding**: Evaluate constant expressions at parse time
2. **Common Subexpression Elimination**: (planned) Cache repeated computations
3. **Tail Call Optimization**: (planned) Efficient recursion
4. **JIT Compilation**: (future) Generate native code for hot paths

## Future Extensions

### Planned Features

1. **Module System**: Import/export definitions
2. **Pattern Matching**: Destructuring and guards
3. **Macros**: Syntax transformations
4. **Continuations**: First-class control flow
5. **Parallel Evaluation**: Multi-threaded execution
6. **Distributed Evaluation**: Multi-node computation

### Research Directions

1. **Homomorphic Operations**: Compute on encrypted data
2. **Zero-Knowledge Proofs**: Prove computation correctness
3. **Differential Privacy**: Add noise to preserve privacy
4. **Secure Multi-Party Computation**: Multiple parties with private inputs
5. **Verified Compilation**: Formally verified code generation

## Appendix: Grammar

```bnf
program     ::= expr*
expr        ::= atom | list
atom        ::= number | string | symbol | hole
list        ::= '(' expr* ')'
number      ::= [+-]? digit+ ('.' digit+)? ([eE] [+-]? digit+)?
string      ::= '"' char* '"'
symbol      ::= [a-zA-Z+\-*/<>=!?][a-zA-Z0-9+\-*/<>=!?]*
hole        ::= '?' symbol | '?' '{' symbol ':' constraint '}'
constraint  ::= symbol ('(' args ')')?
```