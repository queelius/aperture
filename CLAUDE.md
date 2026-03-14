# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Aperture is a minimal Lisp-like language where "holes" (written `?x` or `?ns.x`) represent unknown values that can be filled later. This enables pausable, resumable evaluation for coordinating computation across distributed systems.

**This is a coordination primitive, not a security mechanism.** Apertures leak information through program structure and are suitable for honest-but-curious settings where coordination matters more than cryptographic privacy.

## Build Commands

```bash
go build -o aperture ./cmd/aperture      # Build the CLI
go test ./...                             # Run all tests
go test -race ./...                       # Run with race detector
go test ./pkg/eval/... -run TestPartialEval  # Run a single test
go test -coverprofile=coverage.out ./...  # Test coverage
go tool cover -html=coverage.out          # View coverage report
go install ./cmd/aperture                 # Install globally
```

## Architecture

### Evaluation Pipeline

The core data flow is a three-phase cycle:

1. **Parse** (`parser.Parse`) - S-expression string to `ast.Expr` tree
2. **PartialEval** (`eval.PartialEval`) - Evaluate as far as possible, preserving holes; applies algebraic simplifications
3. **Fill** (`eval.Fill`) - Substitute holes with concrete values, producing a new `ast.Expr`
4. **Eval** (`eval.Eval`) - Full evaluation of ground expressions to `value.Value`

Phases 2-4 can repeat: partial-eval, fill some holes, partial-eval again, fill more, eventually fully evaluate.

### Dual-Layer Type System

Two parallel type hierarchies connected by conversion functions:

- **`ast.Expr`** (syntax layer): `AtomExpr`, `ListExpr`, `QuoteExpr` - used in parsing, partial evaluation, and serialization. The `AtomExpr` type is a tagged union using `AtomKind` to distinguish nil/num/sym/str/bool/hole.
- **`value.Value`** (runtime layer): `Nil`, `Num`, `Sym`, `Str`, `Bool`, `Hole`, `List`, `Lambda` - used during full evaluation.

Both use the **sealed interface pattern** (unexported marker method) to prevent external implementations.

Bridge functions in `pkg/eval/partial.go`:
- `valueToExpr(v value.Value, pos ast.Pos) ast.Expr` - runtime value back to AST node
- `exprToValue(expr ast.Expr) value.Value` - AST node to runtime value (used by `quote`)

### Partial Evaluation Strategy

`PartialEval` works on AST nodes (not values). When a subexpression is ground (no holes), it tries full evaluation via `safeEval` — a panic-recovery wrapper around `Eval` that returns `nil` on failure. This lets partial evaluation optimistically attempt full evaluation without crashing on undefined variables or type errors.

Algebraic simplifications (`pkg/eval/simplify.go`) are applied during partial evaluation of arithmetic:
- **Constant folding**: `(+ 3 5)` -> `8`; also across variadic args: `(+ 3 ?x 5)` -> `(+ 8 ?x)`
- **Identity**: `(+ 0 ?x)` -> `?x`, `(* 1 ?x)` -> `?x`
- **Annihilation**: `(* 0 ?anything)` -> `0`

### Error Handling

Panics on errors throughout (research prototype). The `safeEval` function in `partial.go` uses `defer/recover` to catch panics during opportunistic evaluation within partial-eval.

### Package Structure

```
cmd/aperture/       # CLI: REPL + subcommands (run, eval, partial, fill)
pkg/
├── ast/            # AST types, hole detection (HasHoles, CollectHoles)
├── parser/         # Hand-written recursive descent S-expression parser
├── eval/           # eval.go (full), partial.go (partial + Fill), simplify.go
├── value/          # Runtime value types with sealed interface
├── env/            # Lexically scoped environment chain (parent pointers)
├── trace/          # Structured JSON tracing for fill events
└── serialize/      # Expression serialization
demo/query/         # HTTP client/server showing the "local computation" pattern
```

### CLI

```bash
./aperture                                    # REPL
./aperture eval "(+ 1 2)"                     # Full evaluation
./aperture partial examples/template.apt      # Partial evaluation
./aperture fill --hole x=10 --hole y=5 file.apt  # Fill holes then evaluate
./aperture run examples/query.apt             # Execute file
```

## Testing

Tests are table-driven in `pkg/eval/eval_test.go`. Categories: arithmetic, lambda/closures, let bindings, conditionals, partial evaluation (hole preservation + simplification), hole filling, hole extraction, define.

## Reference

- Full language specification: `SPEC.md`
- Workshop paper: `paper/workshop/apertures-workshop.tex`
