# Methodology Auditor Report

**Paper**: Apertures: Coordinated Partial Evaluation for Distributed Computation
**Date**: 2026-03-14
**Specialist**: methodology-auditor

## Overall Assessment

The paper's experimental methodology is fundamentally compromised by the mismatch between the reported implementation (C++) and the actual codebase (Go). The benchmarks are unreproducible, the methodology is underdescribed, and the experimental claims are unsupported by the available evidence.

## Findings

### CRITICAL-1: Reported benchmarks are from a defunct implementation

**Location**: Section 4, Performance Characteristics table
**Severity**: Critical
**Confidence**: High

**Quoted text**: "Our C++ implementation provides: S-expression parser with aperture syntax, Partial evaluator with algebraic simplification, Hole tracking and filling mechanism, Fluent API for natural expression construction"

And: "Key design choices: Shared pointer memory management, Visitor pattern for evaluation, Variant-based value representation, Stack-based evaluation with continuation frames"

The paper describes a C++ implementation with specific design choices (shared pointers, visitor pattern, variant-based values, stack-based evaluation). However, the repository contains a Go implementation with entirely different architecture:
- Go interface-based value types (not variants)
- Recursive descent evaluation (not stack-based with continuations)
- Garbage-collected memory (not shared pointers)
- Type-switch dispatch (not visitor pattern)

The performance characteristics of these two architectures are fundamentally different. The reported 2-5% overhead numbers cannot be verified or reproduced from the current codebase.

**Suggestion**: Either (a) restore and include the C++ implementation alongside its benchmarks, (b) re-run benchmarks on the Go implementation and update the paper, or (c) remove the benchmarks entirely and frame the paper as a design/patterns paper without performance claims.

### MAJOR-1: No benchmark methodology description

**Location**: Section 4.1
**Severity**: Major
**Confidence**: High

The performance table reports microsecond-level measurements but provides no information about:
- Hardware and OS used
- Number of trials / statistical measures (mean? median? std dev?)
- Warm-up period
- Compiler/interpreter version and optimization flags
- Whether GC pauses were accounted for
- What "Arithmetic (1000 ops)" means precisely -- is this 1000 additions? Mixed operations?

Microsecond-level benchmarks are notoriously sensitive to measurement methodology. Without this information, the numbers are not reproducible or interpretable.

**Suggestion**: If benchmarks are retained, add a standard methodology description. For workshop papers, a brief paragraph suffices: hardware, measurement approach, number of trials, and variance.

### MAJOR-2: Benchmark comparison is not meaningful

**Location**: Section 4.1
**Severity**: Major
**Confidence**: High

The table compares "Direct" vs. "With Apertures" timing for operations like "Arithmetic (1000 ops)", "List processing", etc. But both columns measure execution in the same custom language. The meaningful comparison for a coordination primitive would be:
- Aperture-based coordination vs. alternative coordination mechanisms (message passing, shared memory, etc.)
- End-to-end latency of a distributed workflow with and without apertures
- Overhead of serialization/deserialization for sending partially evaluated expressions across the network

Showing that partial evaluation adds 3% overhead to arithmetic is not informative for the paper's claims about distributed coordination.

**Suggestion**: Replace micro-benchmarks with at least one end-to-end measurement of the query optimization scenario (Section 5) using the actual HTTP demo in the repository.

### MAJOR-3: Case study is entirely hypothetical

**Location**: Section 5
**Severity**: Major
**Confidence**: High

The case study describes a "distributed database scenario" using pseudocode with undefined operations (`server-optimize`, `local-execute`). Despite the repository containing an actual HTTP-based demo (`demo/query/`), the paper presents only hypothetical code. The case study makes claims about what the server "can" do (reorder predicates, plan aggregation, choose join algorithms, prepare indexes) without any of these being implemented.

**Suggestion**: Use the actual working demo from the repository. Report real measurements. Even if the demo is simple, a working system with real numbers is far more convincing than hypothetical pseudocode.

### MINOR-1: "Local computation pattern" benefits are asserted without evidence

**Location**: Section 3.2
**Severity**: Minor
**Confidence**: High

The paper lists benefits:
- "Server CPU used for optimization"
- "Data never leaves client"
- "Results stay local"
- "Reduced leakage channels"

The first three are architectural properties of the pattern, not empirical findings. The fourth ("Reduced leakage channels") is a claim that would require analysis to support -- what channels exist, which are reduced, and by how much?

**Suggestion**: Be precise about which claims are definitional (data stays local by construction) vs. empirical (reduced leakage) vs. aspirational (server CPU used for optimization -- but what optimization does the implementation actually do?).

### MINOR-2: Speculative compilation pattern is entirely hypothetical

**Location**: Section 3.3
**Severity**: Minor
**Confidence**: High

The speculative compilation pattern uses `compile-all-paths`, a function that does not exist in the implementation. The paper presents this as a coordination pattern but it is actually a hypothetical future direction. There is no compilation in the aperture system.

**Suggestion**: Either implement this pattern or reframe it explicitly as future work rather than a current capability.

## Reproducibility Assessment

| Aspect | Status |
|--------|--------|
| Source code available | Yes (Go, not C++ as described) |
| Build instructions | Yes |
| Test suite | Partial (34% coverage, only eval package) |
| Benchmark code | Not available |
| Benchmark data | Not reproducible |
| Case study | Not reproducible (hypothetical) |
| Demo system | Available but not used in paper |

## Summary

| Severity | Count |
|----------|-------|
| Critical | 1 |
| Major    | 3 |
| Minor    | 2 |

The methodology issues are severe. The paper makes performance claims based on unreproducible benchmarks from a defunct implementation, presents a hypothetical case study despite having a working demo available, and does not provide sufficient detail for any experimental claim to be verified. For a workshop paper, the fix is straightforward: use the Go implementation, run the demo, report real numbers, and be transparent about what is implemented vs. hypothetical.
