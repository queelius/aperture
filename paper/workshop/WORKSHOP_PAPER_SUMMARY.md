# Workshop Paper: "Apertures: Coordinated Partial Evaluation for Distributed Computation"

## Overview
Created a focused 9-page workshop paper suitable for submission to PL workshops (PEPM, PADL, etc.)

## Key Changes from Original Paper

### Reframing
- **From:** Security/privacy mechanism with cryptographic claims
- **To:** Coordination mechanism for distributed computation
- **Explicit:** No security guarantees, apertures leak information

### Content Focus
- Removed all security proofs and cryptographic comparisons
- Added explicit "When NOT to use apertures" section (full page)
- Emphasized local computation pattern as primary use case
- Honest about information leakage throughout

### Structure (9 pages total)
1. **Introduction (1 page)**: Coordination focus with critical disclaimer
2. **The Aperture Language (2 pages)**: Simplified core calculus
3. **Coordination Patterns (2 pages)**: Progressive refinement, local computation, speculative compilation
4. **Implementation Highlights (1 page)**: Performance numbers showing 2-5% overhead
5. **Case Study: Query Optimization (1.5 pages)**: Concrete distributed database example
6. **Related Work (0.5 pages)**: Brief comparison with partial evaluation, MPC, etc.
7. **Limitations (1 page)**: Explicit scenarios where apertures are inappropriate
8. **Conclusion (0.5 pages)**: Summary and future work
9. **References (0.5 pages)**

## Key Intellectual Honesty Points

### What We Claim
- Apertures enable pausable/resumable evaluation
- Support multi-party progressive refinement
- Minimal overhead (2-5%) for coordination
- Useful for non-adversarial distributed settings

### What We DON'T Claim
- No privacy preservation
- No security against adversaries
- Not suitable for sensitive data
- Cannot meet regulatory compliance (GDPR, HIPAA)

### Explicit Limitations Section Covers
- Adversarial settings (no protection)
- High-sensitivity data (too much leakage)
- Regulatory compliance (no guarantees)
- Concrete leakage examples

## Primary Use Case: Local Computation
```lisp
;; Server optimizes without seeing data or results
(let ((plan (server-optimize ?query)))
  (client-execute plan ?local-data))
;; Results stay local, reducing leakage channels
```

Benefits:
- Server CPU used for optimization
- Data never leaves client
- Results stay local
- Reduced (but not eliminated) leakage

## Files Created
- `apertures-workshop.tex` - Main workshop paper (9 pages)
- `references-workshop.bib` - Minimal bibliography
- `apertures-workshop.pdf` - Compiled PDF output

## Submission Notes
This paper is now suitable for workshop submission with:
- Appropriate length (9 pages)
- Intellectual honesty about limitations
- Focus on coordination rather than security
- Concrete use cases and performance numbers
- Clear positioning as a lightweight alternative to heavy cryptographic approaches

The paper acknowledges apertures' limitations while arguing they fill a useful niche for coordination in non-adversarial distributed systems where parties need to control when and where data is introduced.