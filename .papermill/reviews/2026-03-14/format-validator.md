# Format Validator Report

**Paper**: Apertures: Coordinated Partial Evaluation for Distributed Computation
**Date**: 2026-03-14
**Specialist**: format-validator

## Overall Assessment

The paper uses a custom article format rather than a venue-specific template. For PEPM and PADL submission, this would need to be adapted. Several formatting issues exist but none prevent comprehension.

## Findings

### MAJOR-1: Not using venue-specific LaTeX template

**Location**: Line 1 of apertures-workshop.tex
**Severity**: Major
**Confidence**: High

**Quoted text**: `\documentclass[10pt]{article}`

PEPM and PADL are typically co-located with POPL and use ACM conference formats:
- PEPM: ACM `acmart` document class with `sigconf` or `sigplan` option
- PADL: Springer LNCS format (`llncs` document class)

The paper uses plain `article` with custom margins. This would need to be reformatted for submission, and page counts may change significantly (ACM sigplan format produces quite different pagination than plain article).

**Suggestion**: Choose a target venue and reformat using the appropriate template before submission. The ACM template in particular will require `\begin{acks}` section, CCS concepts, keywords, and ACM reference format.

### MINOR-1: No keywords or ACM CCS concepts

**Location**: After abstract
**Severity**: Minor
**Confidence**: High

Neither keywords nor ACM Computing Classification System concepts are provided. Both PEPM (ACM) and PADL (Springer) require these.

**Suggestion**: Add keywords such as: partial evaluation, coordination, distributed computation, holes, staged computation.

### MINOR-2: Listings configuration does not highlight aperture holes

**Location**: Lines 29-41
**Severity**: Minor
**Confidence**: Medium

The lstdefinestyle attempts to highlight `?` with `emph={?}` and brown color, but the lstlistings `emph` feature matches whole tokens, not prefixes. The `?` character alone will rarely appear as a standalone token, so `?variable` will not be highlighted. The brown aperture color is defined but likely never applied.

**Suggestion**: Use `moredelim` or `literate` in lstlistings to highlight the `?` prefix pattern, or use a different approach (e.g., manual `\textcolor` in the listings via `escapeinside`).

### MINOR-3: Code example indentation is inconsistent

**Location**: Listings throughout Sections 3 and 5
**Severity**: Minor
**Confidence**: High

Some code blocks use 2-space indentation, others use 4-space. The Lisp convention is typically 2-space. Some blocks have inconsistent internal indentation (e.g., Section 3.1 listing has mixed alignment).

**Suggestion**: Standardize to 2-space indentation throughout all code listings.

### MINOR-4: Table in Section 4.1 uses microsecond symbol inconsistently

**Location**: Section 4.1, performance table
**Severity**: Minor
**Confidence**: High

The table header uses `($\mu$s)` which renders correctly, but the column headers are quite wide relative to the data. The "Overhead" column has no unit (it is a percentage). This is clear in context but would benefit from a `(\%)` in the header.

**Suggestion**: Add `(\%)` to the Overhead column header for clarity.

### MINOR-5: No figure or table numbering

**Location**: Section 4.1
**Severity**: Minor
**Confidence**: High

The performance table is not a numbered `\begin{table}` float with a caption. It uses `\begin{center}` directly. This means it cannot be cross-referenced and does not appear in a list of tables.

**Suggestion**: Wrap in `\begin{table}[t]` with `\caption{Performance overhead of aperture evaluation}` and `\label{tab:performance}`.

### SUGGESTION-1: Consider adding line numbers for review

For review submission, adding `\usepackage{lineno}` and `\linenumbers` would make it easier for reviewers to reference specific locations in the text.

## Build Status

The paper builds with `pdflatex` + `bibtex` based on the existing .bbl file. The Makefile and build artifacts are present. The .bbl file is committed, which means the paper can be built without running bibtex, but changes to references-workshop.bib will require a bibtex re-run.

## Summary

| Severity | Count |
|----------|-------|
| Critical | 0 |
| Major    | 1 |
| Minor    | 5 |
| Suggestions | 1 |
