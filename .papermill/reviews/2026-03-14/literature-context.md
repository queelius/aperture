# Literature Context

**Paper**: Apertures: Coordinated Partial Evaluation for Distributed Computation
**Date**: 2026-03-14

## Broad Field Survey

### Partial Evaluation in Distributed Systems

The paper's core idea -- partial evaluation with explicit holes for multi-party coordination -- sits at the intersection of several well-established research areas. The current bibliography (5 cited references, 1 uncited) is extremely thin for a workshop paper targeting PEPM or PADL.

**Key missing references in partial evaluation:**

1. **Consel & Danvy (1993)** - "Tutorial Notes on Partial Evaluation" -- the canonical tutorial that PEPM attendees will expect cited alongside Jones et al.
2. **Bondorf (1990)** - "Automatic autoprojection of higher order recursive equations" -- relevant to partial evaluation of higher-order functions (lambdas with holes).
3. **Futamura (1971/1999)** - "Partial Evaluation of Computation Process" -- the Futamura projections are foundational context for any partial evaluation paper.
4. **Thiemann (1999)** - "Combinators for program generation" -- relevant to the template/hole-filling pattern.

### Multi-Stage Programming and Metaprogramming

The paper cites Taha & Sheard (1997) but misses significant follow-up work:

1. **Taha & Nielsen (2003)** - "Environment classifiers" -- directly relevant as it addresses the issue of staged computation across trust boundaries.
2. **Mainland (2012)** - "Explicitly heterogeneous metaprogramming with MetaHaskell" -- heterogeneous staging across different execution environments.
3. **Rompf & Odersky (2010)** - "Lightweight Modular Staging" -- the most influential modern staging framework; comparison would be expected by reviewers.
4. **Kiselyov (2014)** - "The design and implementation of BER MetaOCaml" -- mature implementation of multi-stage programming.

### Coordination Languages and Distributed Computation

This is arguably the paper's weakest bibliographic coverage. Key missing work:

1. **Gelernter (1985)** - "Generative communication in Linda" -- tuple spaces as coordination primitives; the "fill a hole" metaphor is conceptually similar.
2. **Papadopoulos & Arbab (1998)** - "Coordination Models and Languages" -- survey of coordination primitives.
3. **Viroli & Omicini (2006)** - "Coordination as a Service" -- service-oriented coordination.
4. **Hewitt et al. (1973)** - Actor model -- relevant to multi-party computation coordination.
5. **Agha (1986)** - "Actors: A Model of Concurrent Computation" -- actors for distributed coordination.

### Contextual Types and Open Code

The concept of "expressions with holes" has been formalized in type theory:

1. **Nanevski et al. (2008)** - "Contextual modal type theory" -- provides a formal type-theoretic account of "open code" (code with holes), directly relevant.
2. **Davies & Pfenning (2001)** - "A Modal Analysis of Staged Computation" -- modal logic for staging, relevant to the formal semantics.
3. **Cave & Pientka (2012)** - "Programming with binders and indexed data-types" -- higher-order abstract syntax with holes.

### Lightweight Alternatives to Cryptographic Computation

The paper positions itself against FHE and MPC but misses closer alternatives:

1. **Trusted Execution Environments** -- costan2016intel is in the bib but NOT cited in the paper. Intel SGX, ARM TrustZone are the direct "lightweight alternative" competitors.
2. **Differential Privacy (Dwork et al., 2006)** -- another "pragmatic middle ground" approach to privacy.
3. **Secure enclaves and attestation** -- relevant as an alternative "honest-but-curious" primitive.

### Program Slicing and Partial Programs

1. **Weiser (1981)** - "Program Slicing" -- decomposing programs into parts relevant to specific variables.
2. **Tip (1995)** - "A survey of program slicing techniques" -- relevant to the idea of separating "known" from "unknown" parts of programs.

## Targeted Comparisons

### Direct Competitors / Closest Related Work

1. **MetaOCaml's cross-stage persistence** -- allows values to persist across stages, similar to how aperture values can be filled incrementally.
2. **Template Haskell** -- compile-time metaprogramming with holes (splices).
3. **Lisp macros with gensym** -- the traditional Lisp approach to code templates with placeholders.
4. **JavaScript template literals** -- the most widespread "expression with holes" pattern in practice.
5. **Scala's multi-stage programming (Stager)** -- relevant modern implementation.

### Same Problem Space

No prior work was found that combines ALL of:
- Partial evaluation + explicit named holes + multi-party filling + coordination patterns

However, individual pieces exist:
- Symbolic execution (holes as symbolic values)
- Parameterized queries (holes as bind variables)
- Code generation templates (holes as splice points)
- Futures/promises (holes as pending values)

The paper would benefit from explicitly distinguishing apertures from these existing "hole-like" abstractions.

## Summary of Critical Gaps

1. **Only 5 references actually cited** (costan2016intel is in bib but unused) -- far too few for PEPM/PADL
2. **No coordination language references** despite "coordination" being the paper's core framing
3. **No contextual type theory references** despite the formal semantics of "expressions with holes" being well-studied
4. **No modern staging references** (nothing after 1997 in staging)
5. **Missing Futamura projections** -- foundational for any partial evaluation paper
6. **No program slicing / symbolic execution comparison** -- closest technical relatives
7. **costan2016intel in bib but never \cited** -- dead reference
