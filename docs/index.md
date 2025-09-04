# Aperture Language Documentation

Welcome to the Aperture Language documentation. Aperture is a security-focused programming language that enables secure distributed computation through "apertures" - partial program specifications with holes.

## Quick Links

- [Language Design Document](DESIGN.md) - Complete language specification
- [API Reference](api/html/index.html) - Generated API documentation
- [Getting Started Guide](getting-started.md) - Tutorial for new users
- [Security Model](security.md) - Security properties and threat model
- [Examples](examples.md) - Code examples and use cases

## What is Aperture?

Aperture is a functional programming language that formalizes the concept of "incomplete specifications" through apertures. When evaluating expressions with unknown values (holes), the language performs partial evaluation to simplify as much as possible while preserving the unknowns.

### Key Features

- **Privacy-Preserving Computation**: Process data without exposing sensitive values
- **Algebraic Simplification**: Automatic optimization of mathematical expressions
- **Secure Distributed Evaluation**: Untrusted servers can optimize without seeing private data
- **Modern C++ Implementation**: Fast, memory-safe implementation with C++23

## Example

```lisp
; Define a tax calculation with private income
(let ((rate 0.25) (deductions 5000))
  (- (* ?income rate) deductions))

; After server optimization:
(- (* ?income 0.25) 5000)

; Client fills locally:
; income = 100000 → Result: 20000
```

## Installation

### Requirements

- C++23 compiler (GCC 13+ or Clang 16+)
- CMake 3.20+
- Optional: Doxygen for documentation generation

### Building from Source

```bash
git clone https://github.com/queelius/aperture.git
cd aperture
mkdir build && cd build
cmake ..
make -j4
```

### Running Tests

```bash
ctest --output-on-failure
```

## Documentation Structure

- **[Language Design](DESIGN.md)**: Complete specification of syntax, semantics, and features
- **[API Reference](api/html/index.html)**: Doxygen-generated C++ API documentation
- **[Security Model](security.md)**: Threat model and security properties
- **[Getting Started](getting-started.md)**: Tutorial and first programs
- **[Examples](examples.md)**: Real-world use cases and patterns

## Contributing

Aperture is an open-source project. Contributions are welcome! Please see our [Contributing Guide](contributing.md) for details.

## License

Aperture is licensed under the Apache License 2.0. See [LICENSE](../LICENSE) for details.

## Author

Alexander Towell

## Links

- [GitHub Repository](https://github.com/queelius/aperture)
- [Issue Tracker](https://github.com/queelius/aperture/issues)
- [Discussions](https://github.com/queelius/aperture/discussions)