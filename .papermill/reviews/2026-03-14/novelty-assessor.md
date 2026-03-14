# Novelty Assessor Report

**Paper**: Apertures: Coordinated Partial Evaluation for Distributed Computation
**Date**: 2026-03-14
**Specialist**: novelty-assessor

## Overall Assessment

The paper presents a straightforward idea -- partial evaluation with named holes for multi-party coordination -- but does not sufficiently distinguish it from existing work. The positioning as a "coordination primitive" is interesting but underdeveloped. The contributions are incremental rather than novel.

## Contribution Analysis

### Contribution 1: "A simple calculus for partial evaluation with holes"

**Assessment**: Weak novelty
**Confidence**: High

Partial evaluation with explicit holes is not new. The concept of "open expressions" (expressions containing free variables or placeholders) is foundational in lambda calculus, contextual modal type theory (Nanevski et al., 2008), and multi-stage programming (Taha & Sheard, 1997 and later work). The calculus presented here is a subset of standard partial evaluation with a syntactic marker for "static unknowns."

What would make this contribution stronger:
- A type system or static analysis for apertures (e.g., ensuring holes are filled before use)
- A formal comparison with contextual types showing what apertures add or sacrifice
- A novel reduction strategy specific to multi-party scenarios

### Contribution 2: "Coordination patterns for distributed computation"

**Assessment**: Moderate novelty, but pattern catalog is shallow
**Confidence**: High

The three coordination patterns (progressive refinement, local computation, speculative compilation) are the most interesting part of the paper. However:

- **Progressive refinement** is essentially incremental binding, which exists in any system with free variables or futures/promises.
- **Local computation** is the strongest pattern, but it is described only with pseudocode using undefined functions (`server-optimize`, `client-execute`). No actual distributed protocol is specified.
- **Speculative compilation** is described with `case` expressions and hypothetical `compile-all-paths` function. No actual compilation or specialization occurs in the implementation.

The patterns would be stronger with:
- A formal characterization of what coordination properties apertures provide
- Comparison with existing coordination mechanisms (tuple spaces, channels, futures)
- Actual working examples rather than pseudocode

### Contribution 3: "Explicit characterization of when apertures should not be used"

**Assessment**: Appreciated but not a research contribution
**Confidence**: High

While intellectual honesty about limitations is admirable and uncommon, enumerating when a tool should not be used is not typically considered a research contribution. This would be more appropriate as part of a discussion section than as a claimed contribution.

### Contribution 4: "Performance evaluation showing 2-5% overhead"

**Assessment**: Misleading -- benchmarks are from a defunct C++ implementation
**Confidence**: High

The paper reports benchmarks from a C++ implementation, but the repository now contains a Go implementation with completely different architecture (interface-based vs. variant-based, recursive vs. stack-based). The benchmarks cannot be reproduced from the current codebase. Furthermore:

- The benchmark methodology is not described (single-machine? what hardware? how many trials? warm-up?)
- The comparison is "with apertures vs. direct" but both are in the same custom language -- the relevant comparison would be against an existing system
- 2-5% overhead on micro-benchmarks says little about real-world coordination scenarios

## Differentiation from Prior Work

### vs. Multi-Stage Programming (Taha & Sheard, MetaOCaml)

The paper acknowledges MSP exists but claims "apertures provide a simpler mechanism focused on coordination rather than performance." This is not a sufficient differentiation. MetaOCaml allows code with "holes" (splices) to be constructed, passed between functions, and filled -- essentially the same capability. The key difference is that MSP has a type system ensuring well-staged programs, while apertures do not. This could be framed as either a weakness (no safety guarantees) or a strength (simplicity), but the paper does not engage with this tradeoff.

### vs. Futures/Promises

Promises represent "values that will be available later" -- a future is essentially a hole that gets filled. The paper does not discuss this comparison at all, despite the conceptual similarity. The key difference is that apertures preserve expression structure while promises are opaque, but this is never articulated.

### vs. Symbolic Execution

Holes in expressions that block evaluation are exactly symbolic variables in symbolic execution. The paper does not cite or compare with symbolic execution literature at all.

### vs. Parameterized Queries / Prepared Statements

The query optimization case study (Section 5) is remarkably similar to SQL prepared statements with bind variables. The paper's `?threshold`, `?regions` etc. are functionally identical to `$1`, `$2` in prepared statements. The paper does not acknowledge this existing practice.

## Significance

For PEPM/PADL:
- The idea is reasonable but the execution does not meet the bar for novelty
- The formal treatment is too informal for PEPM
- The practical demonstration is too shallow for PADL
- The literature engagement is insufficient for either venue

## Suggestions for Strengthening

1. **Identify the precise delta over MSP**: What can apertures do that MetaOCaml cannot? Multi-party filling? Incremental filling across trust boundaries? Make this explicit.
2. **Formalize coordination properties**: What invariants do apertures maintain? Information hiding properties? Ordering guarantees on fills?
3. **Implement a real distributed scenario**: The query optimization demo exists in the Go codebase -- report results from actually running it, not from hypothetical pseudocode.
4. **Address the futures/promises comparison**: This is the most obvious "why not just use X" question a reviewer will ask.

## Summary

| Aspect | Rating |
|--------|--------|
| Core idea novelty | Low-Medium |
| Contribution clarity | Medium |
| Differentiation from prior work | Low |
| Significance for target venue | Low-Medium |
