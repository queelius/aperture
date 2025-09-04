# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Aperture is a security-focused programming language that uses "apertures" (holes) to enable secure distributed computation. The secure implementation allows untrusted servers to optimize and partially evaluate computations without accessing sensitive data. Expressions with holes (marked with `?variable`) can be partially evaluated, preserving privacy while enabling optimization.

## Build Commands

```bash
# Compile secure language implementation (C++23 required)
g++ -std=c++23 -O2 aperture.cpp demo.cpp -o demo

# Compile and run tests
g++ -std=c++23 -O2 unit_tests.cpp aperture.cpp -o unit_tests
g++ -std=c++23 -O2 integration_tests.cpp aperture.cpp -o integration_tests

# Compile fluent API demos
g++ -std=c++23 -O2 fluent_demo.cpp aperture.cpp -o fluent_demo
g++ -std=c++23 -O2 simple_fluent_demo.cpp aperture.cpp -o simple_fluent_demo

# Compile secure computation demo
g++ -std=c++23 -O2 secure_computation_demo.cpp aperture.cpp -o secure_demo

# Run Python validation tests
python3 validate_tests.py
```

## Code Architecture

### Secure Implementation (`aperture.hpp/cpp`)

1. **Value System**
   - `Value` struct with variant-based type system: `Nil`, `Hole`, `Num`, `Sym`, `Str`, `List`, `Lambda`
   - Holes represent private/unknown values with optional constraints
   - Smart pointer (`shared_ptr`) based memory management
   - Metadata tracking for security (taint analysis, complexity limits)

2. **Parser**
   - Recursive descent parser for S-expressions
   - Hole syntax: `?variable` or `?{variable:constraint}`
   - Scientific notation support for numbers
   - String and symbol parsing with proper escaping

3. **Evaluator** 
   - Stack-based evaluation with frame management
   - Partial evaluation with holes (`partial_eval`)
   - Hole filling mechanism (`fill_holes`)
   - Security limits (max depth, complexity)
   - Built-in operations: arithmetic, conditionals, let bindings, lambdas

### Fluent C++ API (`fluent_api.hpp`)

1. **Expression Builder**
   - `Expr` class with operator overloading for natural syntax
   - Arithmetic operators: `+`, `-`, `*`, `/`
   - Comparison operators returning expressions
   - Lambda creation and application

2. **Computation Classes**
   - `Computation`: Basic evaluation and hole filling
   - `SecureComputation`: Untrusted server workflow support
   - Method chaining for fluent interface

3. **Builder Namespace**
   - `hole()`, `num()`, `sym()` helper functions
   - Shorthand functions: `x()`, `y()`, `z()` for common holes

### Plugin System (`plugins/plugin_interface.hpp`)

1. **Plugin Interface**
   - Base `Plugin` class for extending evaluation
   - `EvaluationContext` provides expression and environment info
   - `HoleFillResult` with confidence scores

2. **Built-in Plugins**
   - `PatternMatchPlugin`: Fill holes based on patterns
   - `MathContextPlugin`: Infer mathematical constants
   - Future: LLM integration for context-aware filling

3. **Plugin Manager**
   - Singleton for plugin registration
   - Auto-fill toggle and confidence thresholds
   - Priority-based plugin evaluation

## Key Features

### Security Properties
- **Data Privacy**: Sensitive values never leave trusted environment
- **Partial Evaluation**: Untrusted nodes optimize without seeing private data  
- **Hole Tracking**: System tracks unfilled variables
- **Algebraic Simplification**: Expressions reduce while preserving holes

### Hole Syntax
```lisp
?variable           ; Simple hole
?{var:constraint}   ; Hole with constraint
(+ ?x 10)          ; Expression with hole
```

### Fluent API Examples
```cpp
using namespace aperture;
using namespace aperture::build;

// Natural arithmetic syntax
auto expr = (hole("x") + num(10)) * num(2);

// Evaluate with hole filling
Computation comp(expr);
double result = comp.fill("x", 5).eval();  // = (5+10)*2 = 30

// Secure computation workflow
SecureComputation secure(expr);
auto optimized = secure.send_to_server();  // Partial eval
double final = secure.fill({{"x", 5}}).eval();
```

### Plugin System Usage
```cpp
// Register a plugin
auto math_plugin = std::make_shared<MathContextPlugin>();
PluginManager::instance().register_plugin(math_plugin);

// Enable auto-fill
PluginManager::instance().set_auto_fill(true);

// Holes like ?pi will be automatically filled
```

## Testing

The codebase includes comprehensive unit and integration tests:
- `unit_tests.cpp`: Tests arithmetic, conditionals, lambdas, let bindings
- `integration_tests.cpp`: Tests complete workflows and security properties  
- `validate_tests.py`: Python validation of test expectations
- Test coverage includes comparison with Python semantics

## Notes

- Requires C++23 compiler (GCC 13+ or Clang 16+)
- Uses modern C++ features: `std::expected`, `std::variant`, `std::shared_ptr`
- Secure implementation focused on privacy-preserving computation
- Plugin system allows extensibility for hole inference (LLM integration planned)