# Algebraic environments

Environments ground free variables, i.e., provide them with a semantics.

For instance, when mathematicans look at a sheet of paper containing
math symbols, in their minds they have some notion about what the
symbols mean. Their shared understanding provides the grounding, the
*closure*, for these symbols.

If they do not know what some of the symbols mean, then these symbols
remain free variables and the mathematical expressions may not
have a clear meaning, or whose meaning may depend upon other factors.
As discussed previously, we call these expressions *apertures*.

Regardless, we can change the meaning of an expression by changing
the environment. For instance, an expression may have a meaningful
evaluation whether a particular set of symbols are binded to
integer values or matrix values. In this way, an aperture can be
understood as a program template, or a generic program, that works
over a range of environments where the the symbols are binded to
values that model a specific concept, e.g., if the mathematical
objects must form a ring and we apply this environment to the
aperture, then it becomes a well define closure.

## Means of combination
Environments, as first-class citizens in Aperture, may be thought
of as a binary relation of type (symbol,exp*).

As a binary relation, we may take unions, intersections, and other
set-theoretic operations.

We can also ask whether symbol is related to exp* in either direction,
or iterate over the elements or ...


## Means of abstraction
