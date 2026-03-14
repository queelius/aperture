# Multi-Agent Review Report

**Date**: 2026-03-14
**Paper**: Apertures: Coordinated Partial Evaluation for Distributed Computation
**Authors**: Alexander Towell (SIUE)
**Target Venues**: PEPM, PADL workshops
**Recommendation**: major-revision

## Summary

**Overall Assessment**: The paper presents a clean idea -- partial evaluation with named holes as a lightweight coordination primitive for distributed computation -- and takes an admirably honest position about its limitations. However, the paper is not ready for submission due to three interrelated problems: (1) the reported C++ implementation and benchmarks are from a defunct codebase while the actual implementation is in Go, making the performance claims unreproducible; (2) the formal semantics contain inconsistencies between the reduction rules and the actual implementation behavior; and (3) the bibliography is severely underdeveloped, with only 5 cited references and entire categories of related work missing. The coordination patterns are the strongest contribution but are presented with hypothetical pseudocode rather than working examples from the existing Go demo.

**Strengths**:
1. The core idea of using partial evaluation with explicit holes for multi-party coordination is intuitive and well-motivated. The "local computation" pattern where a server optimizes expression structure without seeing data or results is a genuinely useful contribution. (novelty-assessor, methodology-auditor)
2. The intellectual honesty about limitations is exceptional. The paper explicitly enumerates scenarios where apertures should NOT be used, provides concrete information leakage examples, and avoids overclaiming. This is rare and commendable. (prose-auditor, novelty-assessor)
3. The paper is generally well-written and accessible. The Lisp-based syntax makes examples easy to follow, and the writing is clear at the sentence level. (prose-auditor)
4. A working Go implementation exists with a clean architecture, tests that pass, and an HTTP demo of the query optimization pattern. The foundation for a strong paper is present in the codebase. (methodology-auditor)

**Weaknesses**:
1. The paper describes a C++ implementation with specific design choices (shared pointers, visitor pattern, variant-based values, stack-based evaluation) that no longer exists. The actual implementation is in Go with a completely different architecture. The benchmarks are therefore unreproducible. (methodology-auditor, prose-auditor)
2. The formal semantics (Section 2.2, equations 3-5) are internally inconsistent: rule (3) preserves evaluation contexts around blocked holes, while rule (5) discards them. Neither matches the actual recursive-descent partial evaluation in the implementation. (logic-checker)
3. The bibliography contains only 5 cited references (1 additional in .bib but unused). Entire categories of highly relevant work are missing: coordination languages, contextual modal type theory, symbolic execution, futures/promises, modern staging frameworks, and the Futamura projections. This is far below the minimum for PEPM (typically 20-40 references) or PADL (typically 15-30). (citation-verifier)
4. The coordination patterns (Section 3) and case study (Section 5) use hypothetical pseudocode with undefined functions despite having a working HTTP demo in the repository. No empirical measurements of actual coordination scenarios are reported. (methodology-auditor, prose-auditor)

**Finding Counts**: Critical: 3 | Major: 9 | Minor: 10 | Suggestions: 3

## Critical Issues

### 1. C++ implementation claims are factually incorrect (sources: methodology-auditor, prose-auditor)
- **Location**: Abstract (line 59); Section 4 (lines 249-265)
- **Quoted text**: "Our C++ implementation shows apertures add minimal overhead (2-5%) compared to direct evaluation" (abstract); "Key design choices: Shared pointer memory management, Visitor pattern for evaluation, Variant-based value representation, Stack-based evaluation with continuation frames" (Section 4)
- **Problem**: The repository contains a Go implementation with interface-based values, recursive evaluation, and garbage collection. The C++ implementation no longer exists. The benchmarks were produced by the C++ version and cannot be reproduced from the current codebase. The abstract, Section 4 text, and performance table all describe a system that does not exist.
- **Suggestion**: Update the paper to describe the Go implementation. Re-run benchmarks on the Go codebase. If C++ numbers are important, provide the C++ source as supplementary material. At minimum, change "Our C++ implementation" to "Our prototype implementation" and note which version produced the benchmarks.
- **Cross-verified**: Yes, by direct inspection of codebase (Go files in pkg/) and paper text. The state file also acknowledges this discrepancy.

### 2. Formal evaluation semantics are internally inconsistent (source: logic-checker)
- **Location**: Section 2.2, equations (3)-(5)
- **Quoted text**: Rule (3): `E[?h] ->_p E[?h]` and Rule (5): `E[(+ ?h n)] ->_p (+ ?h n)`
- **Problem**: Rule (3) states that when a hole blocks evaluation, the entire evaluation context E is preserved around it. Rule (5) shows an example where the outer context E is discarded and only the immediate operator application `(+ ?h n)` is returned. These are contradictory. If E = `(let ((x [])) body)` and the hole appears in position `[]`, rule (3) says the let is preserved, but rule (5) says only the immediate expression containing the hole is returned. The actual Go implementation uses recursive descent that partially evaluates each subexpression independently, which does not match either rule.
- **Suggestion**: For a workshop paper, drop the evaluation context formalism and describe the partial evaluation algorithm directly. Evaluation contexts are appropriate for a full formal treatment with proofs, but the paper does not provide proofs. An algorithmic description ("recursively partially evaluate subexpressions; when a subexpression contains holes, preserve its structure and apply algebraic simplifications") would be more accurate and easier to verify.
- **Cross-verified**: Yes, verified against the paper text and the Go implementation (pkg/eval/partial.go). The implementation performs recursive descent, not context-based reduction.

### 3. Bibliography is critically insufficient (source: citation-verifier)
- **Location**: References section; references-workshop.bib
- **Quoted text**: N/A (absence of citations)
- **Problem**: Only 5 references are cited in the paper. One additional entry (costan2016intel, Intel SGX) is in the .bib file but never cited. Missing categories include: coordination languages (despite "coordination" in the title), contextual modal type theory (the formal study of "expressions with holes"), symbolic execution, futures/promises, modern staging frameworks (nothing after 1997), Futamura projections, program slicing, and prepared statements/parameterized queries. A PEPM paper typically cites 20-40 references; even a short workshop paper should have 15-20.
- **Suggestion**: Expand to at least 15 references. Priority additions: (1) Futamura projections; (2) Rompf & Odersky 2010 (LMS) or Kiselyov 2014 (MetaOCaml) for modern staging; (3) Gelernter 1985 (Linda/tuple spaces) for coordination; (4) Nanevski et al. 2008 for contextual types; (5) any symbolic execution reference; (6) futures/promises from the concurrency literature. Cite costan2016intel or remove it.
- **Cross-verified**: Yes, verified by counting \cite commands in the .tex file and checking the .bbl output.

## Major Issues

### 4. No formal correctness property for partial evaluation (source: logic-checker)
- **Location**: Section 2 (entire)
- **Problem**: The paper never states the fundamental soundness property: that partial evaluation followed by filling produces the same result as filling first and then evaluating. This property is expected in any partial evaluation paper and is essential for the coordination patterns to be meaningful.
- **Suggestion**: State the property explicitly: "For all ground expressions e' and hole bindings B: result(fill(partial(e), B)) = result(fill(e, B))." Note conditions under which it holds (pure, terminating subexpressions).

### 5. Algebraic simplification correctness is not qualified (source: logic-checker)
- **Location**: Section 2.4, equation (6)
- **Quoted text**: "(* 0 ?x) ->_p 0"
- **Problem**: This simplification changes semantics when ?x could be filled with an expression that diverges, produces side effects, or has a type error (e.g., `(* 0 (/ 1 0))` would error without simplification but return 0 with it). The paper says simplifications "maintain evaluation semantics" without qualification.
- **Suggestion**: Add a note that simplifications assume pure, total, well-typed subexpressions. This is a reasonable assumption for the use cases described but should be stated.

### 6. Case study uses hypothetical pseudocode despite working demo existing (source: methodology-auditor)
- **Location**: Section 5
- **Problem**: The query optimization case study uses undefined functions (server-optimize, local-execute) in pseudocode. The repository contains an actual HTTP-based query optimization demo (demo/query/) that could provide real examples and measurements.
- **Suggestion**: Rewrite the case study to use the actual demo. Show actual input/output from the HTTP server. Report real timing measurements.

### 7. Benchmark methodology is undescribed (source: methodology-auditor)
- **Location**: Section 4.1
- **Problem**: No information about hardware, OS, number of trials, statistical measures, warm-up, or compiler version. Microsecond-level measurements require careful methodology description.
- **Suggestion**: If benchmarks are retained, add a standard methodology paragraph. For a workshop paper, one paragraph is sufficient.

### 8. Benchmark comparison is not meaningful for coordination claims (source: methodology-auditor)
- **Location**: Section 4.1
- **Problem**: The table compares direct vs. aperture execution within the same custom language. The paper's contribution is about coordination, but no coordination overhead is measured (serialization, network transfer, multi-party latency).
- **Suggestion**: Add at least one end-to-end coordination measurement using the HTTP demo.

### 9. Related work is superficial and contains inaccuracies (source: citation-verifier)
- **Location**: Section 6
- **Quoted text**: "Unlike traditional partial evaluation which requires all static values upfront, apertures allow incremental filling."
- **Problem**: This characterization of traditional PE is incorrect. Online partial evaluation discovers static values during evaluation; it does not require them upfront. The FHE overhead claim ("1000-10000x") may also be outdated given recent advances.
- **Suggestion**: Accurately characterize each piece of related work before differentiating. Expand each comparison to a short paragraph.

### 10. Paper lacks clear thesis and argument structure (source: prose-auditor)
- **Location**: Sections 1-2
- **Problem**: The paper reads as a system description rather than an argument for a contribution. There is no clear thesis statement of the form "We argue that X because Y, as demonstrated by Z."
- **Suggestion**: Add a clear thesis statement to the introduction. The disclaimer is important but should follow the positive thesis, not precede it.

### 11. Code examples mix real and pseudo syntax (source: prose-auditor)
- **Location**: Sections 3.1-3.3, Section 5
- **Quoted text**: "(fill query {table: \"users\", col1: \"age\", col2: \"salary\"})"
- **Problem**: The fill syntax uses JSON-like `{key: value}` notation that is not part of the aperture grammar defined in Section 2. Undefined functions like `server-optimize` and `execute` appear throughout. Readers cannot tell which code is executable and which is pseudocode.
- **Suggestion**: Clearly distinguish executable aperture code from pseudocode. Use the actual aperture syntax for fill operations.

### 12. Not using venue-specific LaTeX template (source: format-validator)
- **Location**: Line 1 of apertures-workshop.tex
- **Problem**: Uses plain `\documentclass[10pt]{article}` instead of ACM acmart (for PEPM) or Springer LNCS (for PADL).
- **Suggestion**: Choose a target venue and reformat using the appropriate template.

## Minor Issues

### 13. Excessive security disclaimers dilute the contribution (source: prose-auditor)
- **Location**: Throughout (at least 6 separate disclaimers)
- **Suggestion**: Keep the abstract disclaimer and the dedicated limitations section. Remove or reduce the others.

### 14. "Aperture" terminology used inconsistently (source: prose-auditor)
- **Location**: Throughout
- **Suggestion**: Use "aperture" or "hole" for the syntactic construct, "the aperture calculus/system" for the whole language.

### 15. LLM future work item is jarring and out of scope (source: prose-auditor)
- **Location**: Section 8, conclusion
- **Suggestion**: Remove or shorten to one sentence.

### 16. Notation ->_p not formally defined (source: prose-auditor)
- **Location**: Section 2.2
- **Suggestion**: Define on first use.

### 17. Fill operation edge cases not specified (source: logic-checker)
- **Location**: Section 2.3
- **Suggestion**: Clarify what happens when filling introduces new holes, whether fill triggers re-evaluation, and whether double-filling is an error.

### 18. Bibliography entry types are incorrect (source: citation-verifier)
- **Location**: references-workshop.bib
- **Suggestion**: jones1993partial should be @book; taha1997multi should be @inproceedings; costan2016intel should be @techreport.

### 19. Listings do not properly highlight hole syntax (source: format-validator)
- **Location**: lstdefinestyle configuration
- **Suggestion**: Use moredelim or literate to highlight the `?` prefix pattern.

### 20. Performance table is not a numbered float (source: format-validator)
- **Location**: Section 4.1
- **Suggestion**: Wrap in \begin{table} with \caption and \label.

### 21. No keywords or ACM CCS concepts (source: format-validator)
- **Location**: After abstract
- **Suggestion**: Add appropriate keywords.

### 22. Inconsistent code indentation (source: format-validator)
- **Location**: Listings throughout
- **Suggestion**: Standardize to 2-space indentation.

## Suggestions

1. **Use a running example throughout the paper** -- The query optimization scenario could be introduced in Section 1 and carried through each section to improve cohesion. (prose-auditor)

2. **Add a decision framework for when to use apertures** -- The limitations section enumerates negatives; a simple decision tree or classification would be more actionable. (prose-auditor)

3. **Differentiate from futures/promises and prepared statements** -- These are the most obvious "why not just use X" questions reviewers will ask. Apertures differ in preserving and transforming expression structure, but this is never articulated. (novelty-assessor)

## Detailed Notes by Domain

### Logic and Proofs
The formal content is minimal and contains inconsistencies. The evaluation context notation (equations 2-5) is standard but the rules contradict each other regarding context preservation. No correctness properties are stated or proven. The simplification rules lack preconditions. For PEPM, the formal treatment needs to either be done properly or replaced with an algorithmic description. For PADL, the current level of formality might be acceptable if the rules are corrected.

### Novelty and Contribution
The core idea is not highly novel -- partial evaluation with holes is well-studied in theory (contextual types) and practice (staging frameworks). The positioning as a coordination primitive is the novel angle, but it is underdeveloped. The paper does not formally characterize what coordination properties apertures provide, does not compare with existing coordination mechanisms, and does not empirically demonstrate coordination benefits. The "explicit limitations" contribution, while honest, is not a research contribution per se.

### Methodology
The experimental methodology is fundamentally compromised by the C++/Go mismatch. The benchmarks are from a defunct implementation and cannot be reproduced. The case study is hypothetical despite a working demo existing in the repository. No distributed coordination scenario is actually measured. The fix is straightforward: use the Go implementation, run the existing demo, report real numbers.

### Writing and Presentation
The writing is clear at the sentence level but weak in argument structure. The paper describes rather than argues. The excessive disclaimers, while well-intentioned, consume space that would be better used for positive contributions. The code examples problematically mix executable and pseudocode without distinction. The LLM future work item is out of place.

### Citations and References
This is the paper's most severe weakness for submission. Five cited references is far below the minimum for any PL workshop. Entire categories of directly relevant work are absent: coordination languages, contextual types, symbolic execution, futures/promises, modern staging. The related work section is too brief to demonstrate adequate literature engagement. Several .bib entries have incorrect entry types.

### Formatting and Production
The paper does not use a venue-specific template, which would need to change for submission. The performance table should be a numbered float. Listings configuration does not achieve the intended hole highlighting. These are all straightforward fixes.

## Literature Context Summary

The paper sits at the intersection of partial evaluation, coordination languages, and distributed computation. Its bibliography covers only partial evaluation (Jones 1993), staging (Taha & Sheard 1997), and security/privacy alternatives (Gentry 2009, Yao 1982, Sabelfeld & Myers 2003). The most critical gaps are:

1. **No coordination language references** -- Linda/tuple spaces (Gelernter 1985), actors, and coordination models are the direct context for the paper's coordination claims.
2. **No contextual type theory** -- Nanevski et al. (2008) formalized "expressions with holes" rigorously; this is the paper's closest theoretical relative.
3. **No modern staging** -- Nothing after 1997. MetaOCaml, LMS, Template Haskell all postdate the single staging reference and are more relevant comparisons.
4. **No Futamura projections** -- Foundational for any partial evaluation paper.
5. **No symbolic execution** -- The closest technical relative to "expressions with symbolic variables that block concrete evaluation."
6. **No futures/promises** -- The closest practical relative to "values that will be filled in later."

## Review Metadata
- Review conducted by: area-chair (all specialist domains reviewed directly)
- Specialists simulated: logic-checker, novelty-assessor, methodology-auditor, prose-auditor, citation-verifier, format-validator, literature-scout-broad, literature-scout-targeted
- Cross-verifications performed: 3 (all critical findings verified against manuscript and codebase)
- Disagreements noted: 0 (all specialists' findings were consistent; no conflicting assessments)
