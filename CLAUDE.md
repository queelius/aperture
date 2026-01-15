# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Aperture is a minimal Lisp-like language where "holes" (written `?x` or `?ns.x`) represent unknown values that can be filled later. This enables pausable, resumable evaluation for coordinating computation across distributed systems.

**This is a coordination primitive, not a security mechanism.** Apertures leak information through program structure and are suitable for honest-but-curious settings where coordination matters more than cryptographic privacy.

## Build Commands

```bash
# Build the CLI
go build -o aperture ./cmd/aperture

# Run tests
go test ./...

# Run with race detector
go test -race ./...

# Run a specific package's tests
go test ./pkg/parser/...

# Run a single test by name
go test ./pkg/eval/... -run TestPartialEval

# Test coverage
go test -coverprofile=coverage.out ./...
go tool cover -html=coverage.out

# Build and install globally
go install ./cmd/aperture
```

## Code Architecture

### Package Structure

```
cmd/aperture/       # CLI entry point
pkg/
├── ast/            # AST types (Expr interface and node types)
├── parser/         # Recursive descent S-expression parser
├── eval/           # Evaluator, partial evaluator, and simplifier
├── value/          # Value interface and concrete types
├── env/            # Lexically scoped environments
├── trace/          # Structured JSON tracing
└── serialize/      # Expression serialization
demo/query/         # HTTP client/server demo
testdata/           # Golden test files
```

### Key Design Decisions

1. **Value Types**: Go interface with unexported marker method (sealed interface pattern)
2. **Partial Evaluation**: Preserves holes, blocks on hole in if predicate
3. **Simplification**: Moderate (identity/annihilation + constant folding)
4. **Error Handling**: Panic on errors (research prototype simplicity)
5. **Namespaced Holes**: Support `?ns.x` for multi-party scenarios

### Running the CLI

```bash
# REPL mode
./aperture

# Execute file
./aperture run examples/query.apt

# Evaluate expression
./aperture eval "(+ 1 2)"

# Partial evaluate
./aperture partial examples/template.apt

# Fill holes
./aperture fill --hole x=10 --hole y=5 examples/template.apt
```

## Testing

Tests are in `pkg/eval/eval_test.go` using table-driven tests. Key test categories:
- Arithmetic operations
- Lambda and closures
- Let bindings
- Conditionals
- Partial evaluation (hole preservation, simplification)
- Hole filling
- Hole extraction

Run all tests: `go test ./...`

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
result := eval.Eval(expr, e)  // value.Num{Val: 8}

// Partial evaluation
template, _ := parser.ParseOne("(+ 3 ?x 5)")
partial := eval.PartialEval(template, e)  // "(+ 8 ?x)"

// Fill holes
bindings := map[string]value.Value{"x": value.NewNum(10)}
filled := eval.Fill(partial, bindings)
final := eval.Eval(filled, e)  // 18
```

## Client-Server Demo

The `demo/query/` directory shows the "local computation" pattern:

```bash
# Terminal 1: Start server
go run ./demo/query/server

# Terminal 2: Run client
go run ./demo/query/client '(+ 3 ?x 5)' x=10
```

## Reference

- Full specification: `SPEC.md`
- Workshop paper: `paper/workshop/apertures-workshop.tex`
