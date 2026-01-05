# Apertures Language Specification

A Go implementation of the apertures language for coordinated partial evaluation in distributed computation.

## Overview

Apertures is a minimal Lisp-like language where "holes" (written `?x` or `?ns.x`) represent unknown values that can be filled later. This enables pausable, resumable evaluation—useful for coordinating computation across distributed systems where different parties contribute different values.

**This is a coordination primitive, not a security mechanism.** Apertures leak information through program structure, evaluation patterns, and algebraic relationships. They are suitable for honest-but-curious settings where coordination matters more than cryptographic privacy.

## Project Structure

```
aperture/
├── SPEC.md                    # This specification
├── README.md                  # Usage and examples
├── go.mod                     # github.com/queelius/aperture
├── cmd/
│   └── aperture/
│       └── main.go            # CLI entry point
├── pkg/
│   ├── ast/                   # AST types
│   │   └── ast.go
│   ├── parser/                # S-expression parser
│   │   └── parser.go
│   ├── eval/                  # Evaluator and partial evaluator
│   │   ├── eval.go
│   │   ├── partial.go
│   │   └── simplify.go
│   ├── value/                 # Value types
│   │   └── value.go
│   ├── env/                   # Environments and closures
│   │   └── env.go
│   ├── trace/                 # Structured tracing
│   │   └── trace.go
│   └── serialize/             # Expression serialization
│       └── serialize.go
├── demo/
│   └── query/                 # Query optimization demo
│       ├── server/
│       │   └── main.go
│       └── client/
│           └── main.go
├── examples/                  # Paper examples as runnable files
│   └── *.apt
├── paper/
│   └── workshop/
│       └── apertures-workshop.tex
└── testdata/                  # Golden test files
    └── *.apt
```

## Grammar (BNF)

```bnf
program     ::= expr*

expr        ::= atom
              | hole
              | list
              | quote

atom        ::= number
              | symbol
              | string
              | boolean
              | 'nil'

hole        ::= '?' identifier
              | '?' namespace '.' identifier

namespace   ::= identifier

number      ::= '-'? digit+ ('.' digit+)? (('e' | 'E') ('+' | '-')? digit+)?

symbol      ::= identifier

identifier  ::= (letter | special) (letter | digit | special)*

letter      ::= 'a'..'z' | 'A'..'Z'

digit       ::= '0'..'9'

special     ::= '-' | '_' | '+' | '*' | '/' | '<' | '>' | '=' | '!' | '?'

string      ::= '"' char* '"'

char        ::= <any character except '"' or '\'>
              | '\"' | '\\' | '\n' | '\t'

boolean     ::= 'true' | 'false'

list        ::= '(' expr* ')'

quote       ::= '\'' expr
```

## Value Types

Implemented as Go interfaces with concrete types:

```go
type Value interface {
    String() string
    valueMarker()  // unexported, seals the interface
}

type Nil struct{}
type Num struct{ Val float64 }
type Sym struct{ Name string }
type Str struct{ Val string }
type Bool struct{ Val bool }
type Hole struct{ Namespace, Name string }
type List struct{ Elements []Value }
type Lambda struct {
    Params []string
    Body   Expr
    Env    *Env  // captured lexical environment
}
```

## Core Operations

### Special Forms

| Form | Syntax | Semantics |
|------|--------|-----------|
| `define` | `(define name expr)` | Bind `name` to evaluated `expr` in environment |
| `lambda` | `(lambda (params...) body)` | Create closure capturing lexical environment |
| `let` | `(let ((x e1) (y e2)) body)` | Sequential bindings, then evaluate body |
| `if` | `(if pred then else)` | Conditional; blocks if pred is hole |
| `cond` | `(cond (p1 e1) (p2 e2) (else e3))` | Multi-way conditional |
| `quote` | `(quote expr)` or `'expr` | Return unevaluated expression |
| `begin` | `(begin e1 e2 ... en)` | Sequence; return last value |

### Arithmetic (numbers only)

| Op | Syntax | Notes |
|----|--------|-------|
| `+` | `(+ a b ...)` | Sum; identity 0 |
| `-` | `(- a b)` | Subtraction |
| `*` | `(* a b ...)` | Product; identity 1 |
| `/` | `(/ a b)` | Division; panics on zero |

### Comparison

| Op | Syntax | Returns |
|----|--------|---------|
| `=` | `(= a b)` | Numeric equality → bool |
| `<` | `(< a b)` | Less than → bool |
| `>` | `(> a b)` | Greater than → bool |
| `<=` | `(<= a b)` | Less or equal → bool |
| `>=` | `(>= a b)` | Greater or equal → bool |

### Logic

| Op | Syntax | Semantics |
|----|--------|-----------|
| `and` | `(and a b ...)` | Short-circuit AND |
| `or` | `(or a b ...)` | Short-circuit OR |
| `not` | `(not a)` | Boolean negation |

### List Operations (Go-idiomatic names)

| Op | Syntax | Semantics |
|----|--------|-----------|
| `list` | `(list a b c)` | Create list from elements |
| `head` | `(head lst)` | First element |
| `tail` | `(tail lst)` | All but first |
| `append` | `(append lst1 lst2)` | Concatenate lists |
| `length` | `(length lst)` | Number of elements |
| `empty?` | `(empty? lst)` | True if list is empty |
| `nth` | `(nth lst n)` | Element at index n |

### Hole Operations

| Op | Syntax | Semantics |
|----|--------|-----------|
| `holes` | `(holes expr)` | List of unfilled hole names in expr |
| `fill` | `(fill expr bindings)` | Substitute holes with values |
| `ground?` | `(ground? expr)` | True if no unfilled holes |

## Evaluation Semantics

### Full Evaluation

Standard applicative-order evaluation with lexical scoping. Panics on:
- Undefined variable reference
- Type mismatch (e.g., `(+ "a" 1)`)
- Division by zero
- Unfilled holes in strict positions

### Partial Evaluation

Evaluates as far as possible, preserving holes:

```lisp
(partial-eval '(+ 3 ?x 5))
; => (+ 8 ?x)

(partial-eval '(if ?flag 1 2))
; => (if ?flag 1 2)  ; blocks on hole in predicate

(partial-eval '(* 0 ?anything))
; => 0  ; algebraic simplification
```

### Algebraic Simplifications (Moderate)

Applied during partial evaluation:

**Identity rules:**
- `(+ 0 x)` → `x`
- `(* 1 x)` → `x`

**Annihilation rules:**
- `(* 0 x)` → `0`

**Constant folding:**
- `(+ 3 5)` → `8`
- `(if true a b)` → `a`
- `(if false a b)` → `b`

**Associative folding:**
- `(+ 3 ?x 5)` → `(+ 8 ?x)` (collect constants)

### Hole Filling

```lisp
(define expr '(+ ?x (* ?y 2)))
(fill expr '((x . 10) (y . 5)))
; => (+ 10 (* 5 2))
```

With namespaced holes:
```lisp
(define expr '(+ ?client.x ?server.y))
(fill expr '((client.x . 10)))
; => (+ 10 ?server.y)
```

## Principal Tracking

Fill operations record which principal performed them:

```go
type FillEvent struct {
    Principal string
    Hole      string
    Value     Value
    Timestamp time.Time
}
```

The trace log records fill history:
```json
{
  "events": [
    {"principal": "client", "hole": "x", "value": 10, "timestamp": "..."},
    {"principal": "server", "hole": "y", "value": 5, "timestamp": "..."}
  ]
}
```

## Tracing Format (JSON)

Structured trace output for each evaluation step:

```json
{
  "trace": [
    {
      "step": 1,
      "rule": "const-fold",
      "before": "(+ 3 5)",
      "after": "8"
    },
    {
      "step": 2,
      "rule": "identity",
      "before": "(+ 0 ?x)",
      "after": "?x"
    }
  ],
  "result": "(+ 8 ?x)",
  "unfilled_holes": ["x"]
}
```

## CLI Interface

```bash
# REPL mode
aperture

# Execute file
aperture run examples/query.apt

# Evaluate expression directly
aperture eval "(+ 1 2)"

# Partial evaluate file
aperture partial examples/template.apt

# Fill holes and evaluate
aperture fill --hole x=10 --hole y=5 examples/template.apt

# Fill with principal tracking
aperture fill --principal client --hole x=10 examples/template.apt

# Output trace
aperture eval --trace "(+ 3 ?x 5)"
```

### REPL Commands

```
> (define x 10)
x = 10
> (+ x 5)
15
> :holes (+ ?a ?b)
(a b)
> :trace on
Tracing enabled
> :env
x = 10
> :quit
```

## Library API

```go
package aperture

import (
    "github.com/queelius/aperture/pkg/ast"
    "github.com/queelius/aperture/pkg/eval"
    "github.com/queelius/aperture/pkg/parser"
)

// Parse S-expression string to AST
func Parse(input string) (ast.Expr, error)

// Full evaluation (panics on holes)
func Eval(expr ast.Expr, env *Env) Value

// Partial evaluation (preserves holes)
func PartialEval(expr ast.Expr, env *Env) ast.Expr

// Fill holes in expression
func Fill(expr ast.Expr, bindings map[string]Value) ast.Expr

// Fill with principal tracking
func FillAs(expr ast.Expr, bindings map[string]Value, principal string) (ast.Expr, []FillEvent)

// Get unfilled holes
func Holes(expr ast.Expr) []string

// Check if expression has no holes
func IsGround(expr ast.Expr) bool

// Serialize expression to string
func Serialize(expr ast.Expr) string

// Deserialize string to expression
func Deserialize(s string) (ast.Expr, error)
```

## Query Optimization Demo

HTTP/JSON server demonstrating the "local computation" pattern from the paper.

### Server API

```
POST /optimize
Content-Type: application/json

{
  "expression": "(select ?table (where (> ?col ?val)))"
}

Response:
{
  "optimized": "(select ?table (where (> ?col ?val)))",
  "trace": [...],
  "unfilled_holes": ["table", "col", "val"]
}
```

### Client Usage

```bash
# Send template to server for optimization
curl -X POST http://localhost:8080/optimize \
  -d '{"expression": "(select ?table (where (> profit ?threshold)))"}'

# Fill holes locally and execute
aperture fill --hole table=sales --hole threshold=1000 optimized.apt
```

### Demo Files

- `demo/query/server/main.go` - HTTP server that accepts expressions, optimizes, returns
- `demo/query/client/main.go` - Client that sends template, receives optimized, fills locally

## Testing Strategy

Example-driven testing using paper examples as golden tests:

```
testdata/
├── arithmetic.apt      # (+ 3 5) => 8
├── partial.apt         # (+ 3 ?x 5) => (+ 8 ?x)
├── simplify.apt        # (* 0 ?x) => 0
├── lambda.apt          # ((lambda (x) (+ x 1)) 5) => 6
├── closure.apt         # Lexical capture test
├── fill.apt            # Hole filling test
└── query.apt           # Query optimization example
```

Each `.apt` file contains:
```lisp
;; input
(+ 3 ?x 5)

;; expected (partial-eval)
(+ 8 ?x)

;; expected (fill x=10)
18
```

Run tests: `go test ./...`

## Error Handling

The implementation panics on errors (research prototype simplicity):

- **Parse errors**: Malformed S-expressions
- **Type errors**: Wrong argument types (e.g., `(+ "a" 1)`)
- **Undefined variables**: Reference to unbound name
- **Division by zero**: `(/ x 0)`
- **Arity errors**: Wrong number of arguments

Panic messages include source location when available.

## Dependencies

Minimal dependencies for a research prototype:

- Go 1.21+ (for modern generics and slices package)
- No external parsing libraries (hand-written recursive descent)
- Standard library only for core functionality
- Optional: `net/http` for demo server

## Repository Cleanup

Files to **delete** (implementation being replaced):
- All `src/` directory
- All `tests/` directory
- All `benchmarks_src/` directory
- `paper/aperture.tex` and related (arXiv paper)
- All C++ files in root if any remain

Files to **keep**:
- `paper/workshop/apertures-workshop.tex` (authoritative paper)
- `SPEC.md` (this file)
- `README.md` (will be rewritten)
- `.gitignore`

## Implementation Phases

1. **Core types** - Value interface, AST types, Environment
2. **Parser** - Recursive descent S-expression parser
3. **Full evaluator** - Standard eval with all operations
4. **Partial evaluator** - Hole-preserving evaluation
5. **Simplifier** - Algebraic simplification rules
6. **Tracing** - JSON trace output
7. **CLI** - REPL and subcommands
8. **Demo** - HTTP server/client for query optimization

## Version

Specification version: 1.0.0

Based on: "Apertures: Coordinated Partial Evaluation for Distributed Computation" (workshop paper)
