# Citation Verifier Report

**Paper**: Apertures: Coordinated Partial Evaluation for Distributed Computation
**Date**: 2026-03-14
**Specialist**: citation-verifier

## Overall Assessment

The bibliography is severely deficient for a workshop paper. Only 5 references are actually cited in the text (out of 6 in the .bib file), and significant areas of related work are entirely unrepresented.

## Findings

### CRITICAL-1: Bibliography is far too small for a workshop paper

**Location**: References section
**Severity**: Critical
**Confidence**: High

The paper cites only 5 references. For comparison:
- A typical PEPM paper cites 20-40 references
- A typical PADL paper cites 15-30 references
- Even short workshop papers typically cite 10-20 references

Five references signal that the authors have not engaged with the relevant literature. This is the single most likely reason for rejection at PEPM or PADL.

**Suggestion**: Expand bibliography to at least 15-20 references covering: partial evaluation (2-3 more), staged computation (2-3 more), coordination languages (3-4), contextual types (1-2), symbolic execution (1-2), and practical systems with similar capabilities (2-3).

### MAJOR-1: costan2016intel is in the .bib file but never cited

**Location**: references-workshop.bib line 45-49
**Severity**: Major
**Confidence**: High

The entry `costan2016intel` (Intel SGX explained) appears in the bibliography file but is never `\cite`d in the text. It does not appear in the compiled .bbl file, confirming it is unused. This is either a dead reference or an intended citation that was forgotten.

Since the paper positions apertures against cryptographic approaches, Intel SGX (trusted execution environments) is arguably the most relevant comparison -- it is also a "lightweight alternative to FHE" for honest-but-curious settings.

**Suggestion**: Either cite this reference in the Related Work or Limitations section, or remove it from the .bib file.

### MAJOR-2: Related work section is superficial and missing key comparisons

**Location**: Section 6
**Severity**: Major
**Confidence**: High

Each related work entry is a single sentence of description followed by a single sentence of differentiation. This is too brief to demonstrate understanding of the relationship. Specifically:

1. **Jones et al. (1993)**: The paper says "Apertures extend partial evaluation with explicit holes for multi-party coordination. Unlike traditional partial evaluation which requires all static values upfront, apertures allow incremental filling." This is misleading -- traditional partial evaluation does NOT require all static values upfront; online partial evaluation discovers static values during evaluation. The distinction is more nuanced than presented.

2. **Taha & Sheard (1997)**: The paper says "Apertures provide a simpler mechanism focused on coordination rather than performance." This is reductive -- MSP is not just about performance; it is about generating well-typed, well-staged code. The simplification obscures the real relationship.

3. **Gentry (2009)**: Claiming "1000-10000x overhead" for FHE is outdated. Modern FHE implementations (TFHE, SEAL, OpenFHE) have significantly reduced overhead for specific operations. The paper should cite more recent FHE work if making overhead claims.

**Suggestion**: Expand each comparison to at least a short paragraph. Accurately characterize the related work before differentiating. Add missing comparisons (see literature context report).

### MAJOR-3: Missing entire categories of related work

**Location**: Section 6
**Severity**: Major
**Confidence**: High

The following highly relevant areas have zero citations:

1. **Coordination languages** (Linda/tuple spaces, actors) -- despite "coordination" being in the title
2. **Contextual modal type theory** -- the formal study of "expressions with holes"
3. **Symbolic execution** -- expressions with symbolic variables that block concrete evaluation
4. **Futures and promises** -- the most common "fill in a value later" abstraction
5. **Prepared statements / parameterized queries** -- the closest practical relative of the query case study
6. **Modern staging frameworks** (MetaOCaml, LMS, Template Haskell) -- nothing newer than 1997
7. **Futamura projections** -- foundational for any partial evaluation paper

**Suggestion**: Add at least one citation from each of these areas with a clear statement of how apertures differ.

### MINOR-1: Bibliography entries have inconsistent formatting

**Location**: references-workshop.bib
**Severity**: Minor
**Confidence**: High

- `jones1993partial` uses `@inproceedings` but it is actually a book (should be `@book`)
- `taha1997multi` uses `@article` with `journal={Proceedings of ICFP}` -- should be `@inproceedings` with `booktitle`
- `costan2016intel` uses `@inproceedings` with `booktitle={IACR Cryptology ePrint Archive}` -- should be `@techreport` or `@misc`

**Suggestion**: Correct entry types and fields to match standard BibTeX conventions.

### MINOR-2: No self-citations or prior work by the author

**Location**: References
**Severity**: Minor
**Confidence**: Medium

If the author has published related work (e.g., on the C++ implementation, on partial evaluation, on distributed systems), it should be cited. The absence of any self-citations may signal either that this is genuinely the first publication or that related prior work is being overlooked.

**Suggestion**: If prior publications exist on this system, cite them for context and to show evolution of the work.

## Citation Accuracy Check

| Citation | Accuracy | Notes |
|----------|----------|-------|
| jones1993partial | Mostly accurate | Paper correctly describes PE with static/dynamic distinction; characterization of "all static values upfront" is slightly misleading |
| taha1997multi | Accurate | MSP is correctly identified as phase separation |
| gentry2009fully | Partially outdated | FHE overhead claims may not reflect current state of the art |
| yao1982protocols | Accurate | MPC correctly characterized |
| sabelfeld2003language | Accurate | IFC correctly characterized |
| costan2016intel | NOT CITED | Present in .bib but unused |

## Summary

| Severity | Count |
|----------|-------|
| Critical | 1 |
| Major    | 3 |
| Minor    | 2 |
