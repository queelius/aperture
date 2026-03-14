# Prose Auditor Report

**Paper**: Apertures: Coordinated Partial Evaluation for Distributed Computation
**Date**: 2026-03-14
**Specialist**: prose-auditor

## Overall Assessment

The writing is generally clear and readable, with a strong intellectual honesty stance that permeates the paper. However, the paper suffers from structural imbalances, inconsistent terminology, and sections that read more like documentation than academic writing. The narrative arc is weak -- the paper describes what apertures are but does not build a compelling argument for why they matter.

## Findings

### MAJOR-1: Abstract makes a claim about C++ implementation that is no longer accurate

**Location**: Abstract, line 59
**Severity**: Major
**Confidence**: High

**Quoted text**: "Our C++ implementation shows apertures add minimal overhead (2-5%) compared to direct evaluation while enabling novel distributed computation patterns."

The repository contains a Go implementation. This is a factual error in the most prominent part of the paper.

**Suggestion**: Update to reflect the actual implementation language, or describe the implementation language-neutrally ("Our implementation...").

### MAJOR-2: The paper lacks a clear thesis statement and argument structure

**Location**: Sections 1-2
**Severity**: Major
**Confidence**: High

The introduction presents the problem (distributed computation needs coordination), the solution (apertures), and a disclaimer (not a security mechanism). But it never articulates a clear thesis of the form "We argue that X because Y, as evidenced by Z." The contributions list is presented as bullet points without justification.

The paper reads as a description of a system rather than an argument for a contribution. Compare:
- Current: "We present apertures..." (descriptive)
- Better: "We argue that partial evaluation with explicit holes provides a practical coordination primitive for distributed systems because it enables [specific property] with [specific cost], as demonstrated by [specific evidence]."

**Suggestion**: Restructure the introduction to present a clear claim and preview the evidence. The intellectual honesty disclaimer is important but should come after the positive thesis, not before it.

### MAJOR-3: Section 3 coordination patterns use undefined pseudocode

**Location**: Sections 3.1-3.3
**Severity**: Major
**Confidence**: High

The code examples in Section 3 use functions that do not exist in the aperture language: `server-optimize`, `client-execute`, `fill` (with object-method syntax `fill query {...}`), `compile-all-paths`, `execute`. This makes the examples impossible to run and the patterns impossible to verify.

The fill syntax used in Section 3.1:
```
(fill query {table: "users", col1: "age", col2: "salary"})
```
uses JSON-like syntax `{key: value}` that is not part of the aperture grammar defined in Section 2. This is confusing -- is this the real language or pseudocode?

**Suggestion**: Either use actual aperture syntax throughout (and note which parts are executable), or clearly label pseudocode sections as such. Currently, the mixing is confusing.

### MINOR-1: Excessive disclaimers dilute the contribution

**Location**: Throughout
**Severity**: Minor
**Confidence**: Medium

The paper contains at least 6 separate disclaimers about apertures not being a security mechanism:
1. Abstract: "we make no claims about privacy preservation"
2. Introduction para 3: "Critical disclaimer: Apertures are not a security mechanism"
3. Section 2.4: "they also leak information"
4. Section 3.2: "Limitations:" sublist
5. Section 7: entire page of limitations
6. Conclusion: "We make no security claims"

While intellectual honesty is commendable, this level of repetition suggests the paper is defensively overcompensating. It also crowds out space that could be used for positive contributions.

**Suggestion**: Keep the abstract disclaimer and the dedicated limitations section (Section 7). Remove or reduce the others to brief parenthetical notes. Trust the reader to retain the framing after being told twice.

### MINOR-2: "Aperture" terminology is used inconsistently

**Location**: Throughout
**Severity**: Minor
**Confidence**: High

The paper uses "aperture" to mean three different things:
1. The syntactic construct `?variable` (a hole in an expression)
2. The system/language as a whole ("The Aperture Language")
3. The coordination mechanism ("apertures as a coordination primitive")

This is confusing. In photography, an aperture is a hole through which light passes -- the metaphor works for (1) but not for (2) or (3).

**Suggestion**: Use "aperture" or "hole" consistently for the syntactic construct. Use "the aperture system" or "the aperture calculus" for the language/system. Avoid using "apertures" to mean the coordination mechanism -- say "coordination via apertures" instead.

### MINOR-3: The LLM future work item is jarring and out of scope

**Location**: Section 8 (Conclusion), future work list
**Severity**: Minor
**Confidence**: Medium

**Quoted text**: "LLM-based automatic hole inference: Integrating large language models to automatically fill apertures based on context..."

This is the longest item in the future work list and introduces an entirely different research direction. It reads as if it was added opportunistically. It also weakens the paper's positioning -- if holes can be auto-filled by LLMs based on their names, the careful coordination semantics described in the paper become less relevant.

**Suggestion**: Either remove this item or shorten it to one sentence. The other future work items (quantifying leakage, combining with oblivious algorithms) are more coherent with the paper's contributions.

### MINOR-4: Notation in formal sections is non-standard

**Location**: Section 2.2
**Severity**: Minor
**Confidence**: Medium

The paper uses `->_p` for partial evaluation steps but does not define this notation. Standard partial evaluation literature uses different notation (e.g., `~>` for residualization, or explicit "static" and "dynamic" annotations). The subscript `p` is ambiguous -- does it mean "partial" or "preserving"?

**Suggestion**: Define the notation explicitly on first use: "We write `e ->_p e'` to denote a partial evaluation step that may preserve holes."

### SUGGESTION-1: Section 7 (Limitations) could be more analytical

**Location**: Section 7
**Severity**: Suggestion
**Confidence**: Medium

The limitations section enumerates scenarios where apertures are inappropriate but does not provide a framework for reasoning about when they are or are not appropriate. A simple decision tree or classification (e.g., "use apertures when computation structure is public AND parties are honest-but-curious AND latency matters more than privacy") would be more useful than lists of examples.

### SUGGESTION-2: The paper would benefit from a running example

**Location**: Throughout
**Severity**: Suggestion
**Confidence**: High

The paper introduces different examples in different sections. A single running example -- perhaps the query optimization scenario -- introduced in Section 1 and carried through each section would make the paper more cohesive and easier to follow.

## Writing Quality Summary

| Aspect | Rating |
|--------|--------|
| Clarity | Good |
| Organization | Fair (imbalanced) |
| Argument structure | Weak |
| Technical writing | Fair (inconsistent notation) |
| Honesty/Integrity | Excellent |
| Code examples | Poor (mix of real and pseudocode) |
