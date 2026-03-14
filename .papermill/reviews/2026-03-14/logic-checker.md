# Logic Checker Report

**Paper**: Apertures: Coordinated Partial Evaluation for Distributed Computation
**Date**: 2026-03-14
**Specialist**: logic-checker

## Overall Assessment

The paper's formal content is minimal and the logical claims are modest, which limits both the surface area for errors and the strength of the contribution. The calculus presented is standard; the novel claim is about its applicability to coordination, which is argued informally rather than proven.

## Findings

### CRITICAL-1: Evaluation semantics are inconsistent between rules and implementation

**Location**: Section 2.2, equations (3)-(5)
**Severity**: Critical
**Confidence**: High

The paper presents evaluation contexts and reduction rules:

Rule (3): `E[?h] ->_p E[?h]` (aperture blocks)
Rule (4): `E[(+ n1 n2)] -> n1 + n2` (arithmetic)
Rule (5): `E[(+ ?h n)] ->_p (+ ?h n)` (partial)

Rule (3) states that an aperture in ANY evaluation context blocks and the ENTIRE context is preserved. But rule (5) shows the outer context `E` is DISCARDED, and only the immediate application is preserved. These are contradictory. If rule (3) applies, then `(let ((x (+ ?h 1))) (+ x 2))` should block preserving the entire let-expression. If rule (5) applies, the `+` subexpression is extracted from its context.

The actual Go implementation follows neither rule consistently -- it performs a recursive descent that partially evaluates subexpressions independently, which is reasonable but not what the formal rules describe.

**Suggestion**: Either formalize the semantics properly using small-step reduction with explicit stuck states (the standard approach for partial evaluation), or remove the evaluation context notation entirely and describe the approach algorithmically. For a workshop paper, the latter is more appropriate.

### MAJOR-1: Algebraic simplification correctness claim is unsupported for general case

**Location**: Section 2.4, equation (6): `(* 0 ?x) ->_p 0`
**Severity**: Major
**Confidence**: High

The paper states these simplifications "preserve evaluation semantics." However, `(* 0 ?x) -> 0` is only correct if `?x` is guaranteed to be a number (or at least, if the filling of `?x` does not diverge or produce side effects). If `?x` could be filled with a non-numeric expression or a diverging computation, this simplification changes semantics:
- `(* 0 (/ 1 0))` -- without simplification, this raises a division-by-zero error; with simplification, it returns 0
- `(* 0 (launch-missiles))` -- the side effect is suppressed

The paper later mentions (Section 2.4): "if (* ?x e) -> 0 where e is a known non-zero constant, this reveals that ?x = 0" -- this is an information leakage observation, not a correctness concern, and it conflates two different issues.

**Suggestion**: State the preconditions under which simplifications are semantics-preserving: pure expressions, numeric types, terminating computations. Alternatively, note that this is an optimization that trades strict semantic equivalence for practical utility, which is honest and acceptable for the proposed use cases.

### MAJOR-2: No formal statement of partial evaluation correctness

**Location**: Section 2 (entire)
**Severity**: Major
**Confidence**: High

The paper claims apertures enable "pausable and resumable evaluation" but never states a correctness property. The expected property would be something like:

"For all expressions e, environments env, hole names h, and values v:
  result(fill(partial(e, env), h, v)) = result(fill(e, h, v), env)"

That is, partial evaluation followed by filling and evaluation should produce the same result as filling first and then evaluating. This is the fundamental soundness property for any partial evaluation system, and it is never stated, let alone proven.

**Suggestion**: State this property explicitly, even if you do not prove it formally. Note any conditions under which it might not hold (e.g., side effects, non-termination of subexpressions).

### MINOR-1: Fill operation semantics are underspecified

**Location**: Section 2.3
**Severity**: Minor
**Confidence**: High

The paper defines fill as: `fill(e, h, e') = e[e'/?h]`

This is standard capture-avoiding substitution notation, but the paper does not discuss:
1. What happens when filling introduces new holes (e.g., filling `?x` with `(+ ?y 1)`)
2. Whether fill should trigger re-evaluation or just substitute
3. Whether filling the same hole twice is an error

The Go implementation handles case (1) by just substituting and (2) by not re-evaluating, but case (3) is silently allowed (the second fill just finds no matching holes).

**Suggestion**: Briefly clarify these edge cases, particularly since multi-party progressive refinement depends on incremental filling.

### MINOR-2: Evaluation context grammar allows ambiguous decomposition

**Location**: Section 2.2, equation (2)
**Severity**: Minor
**Confidence**: Medium

The evaluation context grammar defines left-to-right evaluation order for application arguments. The paper does not state this is a deterministic decomposition or that the reduction relation is confluent. For a workshop paper this is acceptable, but worth noting.

**Suggestion**: Add a brief note that evaluation is left-to-right and deterministic.

## Summary

| Severity | Count |
|----------|-------|
| Critical | 1 |
| Major    | 2 |
| Minor    | 2 |

The formal content is thin and the rules presented are inconsistent with each other and with the implementation. For a PEPM workshop paper, this is a significant concern -- PEPM reviewers will scrutinize formal semantics carefully. The paper would benefit from either formalizing properly or dropping the formal notation in favor of an algorithmic description.
