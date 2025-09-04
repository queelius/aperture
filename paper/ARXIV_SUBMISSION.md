# arXiv Submission Guide for Apertures Paper

## Paper Details
- **Title**: Apertures: Linguistic Abstractions for Controlled Information Disclosure in Distributed Computation
- **Author**: Alexander Towell (Southern Illinois University Edwardsville)
- **Categories**: cs.CR (Cryptography and Security), cs.PL (Programming Languages), cs.DC (Distributed Computing)
- **Pages**: 37 (single column format)

## Submission Package Contents

### Required Files for arXiv
1. `aperture.tex` - Main LaTeX source (single column, 12pt)
2. `references.bib` - Bibliography file
3. `figures/` - Directory containing all plots:
   - `depth_analysis.pdf` - Computation depth analysis
   - `operation_overhead.pdf` - Performance overhead chart
   - `multiparty_scaling.pdf` - Protocol scaling analysis
   - `llm_accuracy.pdf` - LLM integration accuracy

### Additional Repository Files
- Implementation code (C++23):
  - `../aperture.hpp` - Header file
  - `../aperture.cpp` - Implementation
  - `../benchmarks.cpp` - Experimental benchmarks
- Test suites and demos in parent directory

## Paper Formatting
- **Document class**: `\documentclass[12pt]{article}`
- **Format**: Single column (suitable for arXiv)
- **Font**: Times Roman for better readability
- **Figures**: Embedded PDF plots from experimental results
- **Page count**: 37 pages including appendices

## Key Changes for arXiv
1. ✅ Converted from two-column to single-column format
2. ✅ Adjusted figure sizes (0.65-0.75 textwidth)
3. ✅ Added arXiv categories and keywords
4. ✅ Updated title formatting
5. ✅ Integrated actual experimental results with figures

## Abstract
The paper introduces apertures, a novel programming language construct for controlled information disclosure in distributed computation. Unlike cryptographic approaches, apertures provide a linguistic abstraction where holes in expressions serve as placeholders for private values. The paper acknowledges information leakage through algebraic reasoning and provides formal semantics with explicit leakage quantification.

## Experimental Validation
- Prototype implementation in C++23
- Benchmarks showing 9-25% overhead
- 81-93% computation on untrusted nodes at depth 5
- Multi-party protocols scale to ~20 participants
- LLM integration accuracy: 76-99% depending on domain

## Submission Instructions

1. **Create submission package**:
```bash
tar -czf aperture_arxiv.tar.gz aperture.tex references.bib figures/
```

2. **Upload to arXiv**:
   - Go to https://arxiv.org/submit
   - Select categories: cs.CR (primary), cs.PL, cs.DC
   - Upload the tar.gz file
   - Verify compilation succeeds

3. **Metadata to enter**:
   - Title: As above
   - Authors: Alexander Towell
   - Abstract: Use the abstract from the paper
   - Comments: 37 pages, 4 figures, 5 tables
   - ACM classification: D.4.6 (Security and Protection), D.3.1 (Formal Definitions)

## Source Code Availability
Consider mentioning in the submission comments:
"Source code available at: [GitHub repository URL]"

## License
The paper and code are under appropriate open-source license (check LICENSE file).

## Contact
Alexander Towell - atowell@siue.edu

## Status
✅ Ready for arXiv submission
- Paper compiles without errors
- All figures included and properly sized
- Bibliography complete with 13 references
- Experimental results integrated