# Algebraic expressions

When we evaluate an expression the result is another expression.

Since these expressions represent values, they may be combined and
operated on, in both the host language and in the hosted language.

## Monadic lifting of expressions to values with native data types

In the host language, in this case C++, expressions (values) in Aperture
may be lifted to values in C++, e.g., an expression repesenting a number
may be lifted to, say, double, or a lamba expression in Aperture may
be lifted to a standard function (function object) in C++.

```cpp
// this is not good, revisit
template <typename T>
T lift(aperature::exp * e)
{
    if (is_type<T>(e))
        return value(e);
}
```

