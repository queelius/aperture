# CLAUDE.md

This file provides guidance to Claude Code when working with code in this repository.

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

Golden tests in `testdata/*.apt` contain:
- Input expression
- Expected partial-eval result
- Expected fill result

Run tests: `go test ./...`

## Reference

- Full specification: `SPEC.md`
- Workshop paper: `paper/workshop/apertures-workshop.tex`
