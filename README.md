# Secure Aperture Language

A security-focused programming language that uses "apertures" (holes) to enable secure distributed computation. This allows untrusted servers to optimize and partially evaluate computations without accessing sensitive data.

## Theoretical Foundation

When mathematicians encounter symbols in papers, their mental model provides the binding—the *closure*—for these symbols, grounding them with meaning. Without this binding, symbols remain free variables, and the expression lacks unambiguous meaning. 

Apertures formalize this ambiguity: they are "openings, holes, or gaps" in a program's specification that can be filled later. Unlike traditional systems that fail on undefined variables, apertures make partial specifications first-class citizens.

## Core Concept

The key innovation is treating incomplete program specifications as first-class values. When an expression contains "holes" (marked with `?variable`), evaluation proceeds as far as possible, algebraically simplifying the expression while preserving the holes for sensitive data. This enables a new paradigm: `eval(aperture E) = aperture E`, where apertures are values that represent incomplete computations.

## Architecture

```
┌─────────────────┐         ┌──────────────────┐
│  Trusted Client │         │ Untrusted Server │
├─────────────────┤         ├──────────────────┤
│ • Private data  │────────>│ • Partial eval   │
│ • Fill holes    │ Formula │ • Optimization   │
│ • Final eval    │<────────│ • No access to   │
│                 │Optimized│   private data   │
└─────────────────┘         └──────────────────┘
```

## Language Features

### Value Types
- **Nil**: Empty value
- **Numbers**: Double-precision floating point
- **Symbols**: Variable names and operators
- **Strings**: Text literals
- **Lists**: S-expressions `(op arg1 arg2 ...)`
- **Lambdas**: Functions with closures
- **Holes**: Placeholders for private data `?variable`

### Core Operations
- Arithmetic: `+`, `-`, `*`, `/`
- Conditionals: `if`
- Bindings: `let`, `lambda`
- Partial evaluation with algebraic simplification

## Security Properties

1. **Data Privacy**: Sensitive values never leave the trusted environment
2. **Computation Integrity**: Clients can verify the structure of returned expressions
3. **Partial Evaluation**: Untrusted nodes can optimize without seeing private data
4. **Hole Tracking**: System tracks which variables remain unfilled
5. **Algebraic Simplification**: Expressions reduce to normal form while preserving holes

## Example: Tax Calculation

```lisp
;; Public formula with private income
(let ((rate 0.25) (deductions 5000))
  (- (* ?income rate) deductions))

;; After server optimization:
(- (* ?income 0.25) 5000)

;; Client fills locally:
;; income = 100000 → Result: 20000
```

## Advanced Example: Symbolic Computation

```lisp
;; Expression with free variable y
(lambda x (* (+ x ?y) (+ x ?y)))

;; Applied with x=1, keeping y as hole:
(* (+ 1 ?y) (+ 1 ?y))

;; Could be algebraically expanded to:
(+ 1 (* 2 ?y) (* ?y ?y))

;; Or solved symbolically for specific values
```

## Implementation

### Modern C++23 Design
- `std::expected` for error handling
- `std::variant` for type-safe value representation
- `std::shared_ptr` for automatic memory management
- Zero-copy string views for parsing
- Stack-based evaluation for efficiency

### Performance
- ~0.6 microseconds per evaluation (simple expressions)
- ~0.9 microseconds for partial evaluation with holes
- Memory-safe with bounds checking
- DoS protection via complexity limits

## Building

```bash
# Requires C++23 compiler (GCC 13+ or Clang 16+)
g++ -std=c++23 -O2 aperture.cpp demo.cpp -o demo
./demo
```

## Use Cases

1. **Cloud Computing**: Process datasets with sensitive columns
2. **Financial Services**: Risk calculations with private positions
3. **Healthcare**: Dosage calculations with patient data
4. **Machine Learning**: Model inference with private features
5. **Smart Contracts**: Verify logic without revealing inputs

## Future Directions

- **Homomorphic Operations**: Support for encrypted computation
- **Zero-Knowledge Proofs**: Prove computation correctness
- **Distributed Evaluation**: Multi-party secure computation
- **Type System**: Static verification of hole constraints
- **JIT Compilation**: Generate optimized native code
- **Algebraic Environments**: Environments as first-class citizens with set operations
- **LLM Integration**: Using language models to infer hole values from context
- **Monadic Lifting**: Lifting aperture expressions to native C++ types

## Security Considerations

- Complexity attacks prevented via computation limits
- Side-channel resistance through constant-time operations
- Hole sealing prevents unauthorized modifications
- Taint tracking identifies values derived from holes

This implementation demonstrates that apertures can serve as a practical security primitive for distributed computation, enabling a new class of privacy-preserving applications.