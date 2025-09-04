# Aperture Technical Paper

This directory contains the academic paper describing the Aperture language and its novel approach to privacy-preserving computation.

## Building the Paper

To compile the LaTeX document to PDF:

```bash
cd paper
pdflatex aperture.tex
bibtex aperture
pdflatex aperture.tex
pdflatex aperture.tex  # Run twice for references
```

Or simply:
```bash
make  # If you create a Makefile
```

## Paper Contents

The paper presents:

1. **Core Concept**: Apertures as linguistic abstractions for privacy boundaries
2. **Formal Semantics**: The aperture calculus with operational semantics
3. **Pausable Evaluation**: Stack-based architecture enabling pause/resume at aperture boundaries
4. **Computation Depth Analysis**: How strategic aperture placement maximizes untrusted computation
5. **Multi-Party Progressive Refinement**: Novel round-robin evaluation protocols
6. **Symbolic Inference**: Integration with LLMs for contextual hole-filling
7. **Implementation**: Architecture and optimization techniques
8. **Evaluation**: Performance comparisons showing 100-1000× speedup over FHE
9. **Case Studies**: Real-world applications in healthcare, finance, and regulatory compliance

## Key Insights

- **Structural Privacy**: Privacy through evaluation control rather than encryption
- **Computation Depth**: 70-90% of computation can occur on untrusted infrastructure
- **Pausable/Resumable**: Stack-based evaluation naturally supports distributed computation
- **Symbolic Integration**: Apertures provide a natural interface for hybrid symbolic-neural computation

## Citation

If you use this work in your research, please cite:

```bibtex
@inproceedings{towell2025aperture,
  title={Apertures: Linguistic Abstractions for Privacy-Preserving Computation},
  author={Towell, Alexander},
  booktitle={Proceedings of PLDI 2025},
  year={2025},
  organization={ACM}
}
```

## Author

Alexander Towell  
PhD Student, Computer Science  
Southern Illinois University Edwardsville/Carbondale  
Email: atowell@siue.edu, lex@metafunctor.com