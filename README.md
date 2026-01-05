# Apertures

A minimal Lisp-like language where "holes" (written `?x` or `?ns.x`) represent unknown values that can be filled later. This enables pausable, resumable evaluation for coordinating computation across distributed systems.

**This is a coordination primitive, not a security mechanism.** Apertures leak information through program structure and are suitable for honest-but-curious settings where coordination matters more than cryptographic privacy.

## Installation

```bash
go install github.com/queelius/aperture/cmd/aperture@latest
```

Or build from source:

```bash
git clone https://github.com/queelius/aperture
cd aperture
go build -o aperture ./cmd/aperture
```

## Quick Start

```bash
# Start REPL
aperture

# Evaluate an expression
aperture eval "(+ 3 5)"
# => 8

# Partial evaluation with holes
echo "(+ 3 ?x 5)" > template.apt
aperture partial template.apt
# => (+ 8 ?x)

# Fill holes and evaluate
aperture fill --hole x=10 template.apt
# => 18
```

## Core Concepts

### Holes (Apertures)

Holes are placeholders for unknown values:

```lisp
?x           ; Simple hole
?client.x    ; Namespaced hole (for multi-party scenarios)
```

### Partial Evaluation

Expressions with holes are evaluated as far as possible:

```lisp
(+ 3 ?x 5)        ; => (+ 8 ?x)    ; Constants folded
(* 0 ?anything)   ; => 0           ; Annihilation rule
(* 1 ?x)          ; => ?x          ; Identity rule
(if true ?x ?y)   ; => ?x          ; Conditional elimination
```

### Hole Filling

Fill holes to complete evaluation:

```lisp
; Given: (+ 8 ?x)
; Fill x=10
; Result: 18
```

## CLI Commands

```bash
aperture                        # Start REPL
aperture run <file>             # Execute a file
aperture eval "<expr>"          # Evaluate an expression
aperture partial <file>         # Partial evaluate a file
aperture fill --hole x=10 <file> # Fill holes and evaluate
aperture version                # Show version
```

### REPL Commands

```
> (define x 10)
> (+ x 5)
15
> :holes (+ ?a ?b)
a b
> :partial (+ 3 ?x 5)
(+ 8 ?x)
> :env
x = 10
> :quit
```

## Language Reference

### Data Types

- **Numbers**: `42`, `3.14`, `-5`, `1e10`
- **Strings**: `"hello"`, `"with\nnewline"`
- **Booleans**: `true`, `false`
- **Symbols**: `foo`, `my-var`
- **Lists**: `(1 2 3)`, `(list a b c)`
- **Holes**: `?x`, `?ns.name`

### Special Forms

```lisp
(define name value)             ; Define a variable
(define (name args) body)       ; Define a function
(lambda (args) body)            ; Anonymous function
(let ((x 1) (y 2)) body)        ; Local bindings
(if pred then else)             ; Conditional
(cond (p1 e1) (p2 e2) (else e)) ; Multi-way conditional
(quote expr) or 'expr           ; Quote (don't evaluate)
(begin e1 e2 ... en)            ; Sequence
```

### Arithmetic

```lisp
(+ a b ...)    ; Addition
(- a b)        ; Subtraction
(* a b ...)    ; Multiplication
(/ a b)        ; Division
```

### Comparison

```lisp
(= a b)        ; Equality
(< a b)        ; Less than
(> a b)        ; Greater than
(<= a b)       ; Less or equal
(>= a b)       ; Greater or equal
```

### Logic

```lisp
(and a b ...)  ; Short-circuit AND
(or a b ...)   ; Short-circuit OR
(not a)        ; Negation
```

### List Operations

```lisp
(list a b c)   ; Create list
(head lst)     ; First element
(tail lst)     ; All but first
(append l1 l2) ; Concatenate
(length lst)   ; List length
(empty? lst)   ; True if empty
(nth lst n)    ; Element at index
```

### Hole Operations

```lisp
(holes expr)   ; List hole names in expr
(ground? expr) ; True if no holes
```

## Client-Server Demo

The `demo/query/` directory contains a client-server demo showing the "local computation" pattern from the paper:

```bash
# Terminal 1: Start server
go run ./demo/query/server

# Terminal 2: Run client
go run ./demo/query/client '(+ 3 ?x 5)' x=10
```

Output:
```
=== Step 1: Send to server for optimization ===
Original: (+ 3 ?x 5)
Optimized: (+ 8 ?x)
Unfilled holes: [x]

=== Step 2: Fill holes locally ===
  x = 10

After fill: (+ 8 10)

=== Step 3: Evaluate locally ===
Result: 18
```

## Library Usage

```go
import (
    "github.com/queelius/aperture/pkg/parser"
    "github.com/queelius/aperture/pkg/eval"
    "github.com/queelius/aperture/pkg/env"
    "github.com/queelius/aperture/pkg/value"
)

// Parse and evaluate
expr, _ := parser.ParseOne("(+ 3 5)")
e := env.Global()
result := eval.Eval(expr, e)
// result = value.Num{Val: 8}

// Partial evaluation
template, _ := parser.ParseOne("(+ 3 ?x 5)")
partial := eval.PartialEval(template, e)
// partial.String() = "(+ 8 ?x)"

// Fill holes
bindings := map[string]value.Value{"x": value.NewNum(10)}
filled := eval.Fill(partial, bindings)
final := eval.Eval(filled, e)
// final = value.Num{Val: 18}
```

## When to Use Apertures

**Good for:**
- Coordinating distributed computation where parties already have partial trust
- Scenarios where query structure isn't sensitive
- Local computation with remote optimization assistance
- Educational tools for teaching distributed computing concepts

**NOT for:**
- Adversarial environments (no protection against malicious parties)
- Scenarios requiring cryptographic privacy guarantees
- High-stakes privacy requirements (medical records, financial data)
- Any use case where information leakage is unacceptable

## Documentation

- [SPEC.md](SPEC.md) - Full language specification
- [paper/workshop/](paper/workshop/) - Workshop paper on apertures

## License

MIT
