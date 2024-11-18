# Aperture: Formalizing Ambiguity

## Introduction
When mathematicans look at papers containing math symbols,
they have some notion about what the symbols mean. In this case, their mental model is providing the binding, the *closure*, for these symbols. We often say that these symbols are *grounded*.

If the mathetician does not know what some of the symbols mean,
then these symbols are free variables, in which case the
mathematical expression does not have an unambiguous meaning.

## Expressions and Values
Suppose we have expressions and a way of rewriting them. A common objective is to rewrite an expression towards some form that is self-evaluating, i.e., a value. For instance, we may rewrite the expression `(sum 1 2)` to `3`, which is a value (self-evaluating expression).

Normally, we call this process *evaluation*, and the result of the evaluation is a *value*. We denote this process with the function `eval : (expression, env) -> value`, where `expression` is any expression in the language and `env` is an environment. For simplicity, we drop the `env` in the subsequent discussion.

We see that `(eval (sum 1 2))` maps to `3` and `(eval 3)` maps to `3`. We say that `3` is the normal form of all expressions that evaluate to `3`. Once you read `3` in a mathematical expression, you know what it means, and further evaluation of that expression is not only unnecessary, but will only produce the same result: `(eval(eval(3))) = (eval(3)) = (eval(3)) = ...`

We have just described a program as expression rewriting, where the result of the rewriting is a value. This is a powerful way of thinking about programs, and it is the basis of many programming languages like Scheme.

A value can really be any kind of data type, but normally they are objects like numbers, lists, or Booleans. A function is also a value: `(eval (lambda x x))` maps to `lambda x x`.

## Apertures
When you evaluate an expression and the result is a value, then
we have a closure, a complete specification of the program: the result is a self-evaluating expression, and we know what it means by convention or by definition.

If we evaluate an expression with free variables, normally this results in undefined behavior or an error, e.g., suppose we have the expression `(lambda x (* (+ x y) (+ x y)))`. When we evaluate it, we might get an error about `y` being undefined in the current environment.

We propose one simple addition to the language. When we evaluate an expression and the result is an expression that is not self-evaluating under the rules of the language, then we have what we call an *aperture*, which is an incomplete program specification. However, it is a value: `(eval (aperture E)) = (aperture E)`.

An aperture is "an opening, hole, or gap." So, if we have an aperature, there are *holes* in the program's specification that can be filled in at some other time, i.e., we can pass an aperture around, which is a partial program specification, and evaluate it in some other context where either the expression evaluates to a value or an aperture.

For example, if `y` is not defined in the environment, then if we evaluate `(eval (lambda x (* (+ x y) (+ x y))))`, we get the aperture `(aperture (lambda x (* (+ x y) (+ x y))))`. 
There is, however, another feature: whe we evaluate an expression, it still attempts to rewrite to a normal form as much as possible, like an algebraic simplification. For instance, `(eval (sum (1 2 y)))` maps to `(eval (sum (3 y)))`, assuming `sum` has the normal definition on integers.

### Operations on Apertures

#### Close

What can we do with this? We could force it to the closure:
```lisp
(lambda x y (* (+ x y) (+ x y)))
```
This is not an aperture since it is self-evaluating.
When we evaluate
```lisp
(apply (lambda x y (* (+ x y) (+ x y))) 1 2)
```
we get the value
```lisp
(number 9)
```

#### Applying Environments
However, we could also evaluate it in some other context, e.g.,
by letting all free variables denote symbols for unknown values, and then when we evaluate it, we get the value
```lisp
(lambda x (* (+ x (symbol y)) (+ x (symbol y))))
```
When we evaluate
```lisp
(apply (lambda x (* (+ x (symbol y)) (+ x (symbol y))) 1))
```
we get the value:
```lisp
(* (+ 1 (symbol y)) (+ 1 (symbol y)))
```

We may then, say, evaluate the expression
```
(expand (* (+ 1 (symbol y)) (+ 1 (symbol y))))`
```
which obtains the value
```
(+ 1 (* 2 y) (* y y))
```
we could do other things, like
```
(solve(* (+ 1 (symbol y)) (+ 1 (symbol y)), y, 0))
```
which finds the set of values for `y` such that `(1+y)^2 = 0`.

We see that the aperture is just a partial program specification, where any sub-expression in the aperture is reduced to its most complete specification (values whenever possible).

## Generic programming
Programs can work on expressions, including aperture expressions, so we may also think of apertures as a program template "to be filled in" at some later point.

Apertures, thus, may be seen as a partial program whose complete
specification is a function of the free variables.

One way to bind a free variable in an aperture is to pass it an
additional environment that bounds it to something. Such environments can either be explicitly passed around as first-class citizens, or more likely, the current environment may
be extended with a "let" statement.

For instance, suppose `E` is an aperture expression with `x` and `y`
being free variables. Then,
```scheme
(let ((x 1) (y 2)) (E))
```
is a closure. If, instead, we close only `x`,
```scheme
(let ((x 1)) (E))
```
then the result is another aperture expression with a free variable y.



## Monadic Lifting

Lifting expressions to values with native data types. A big advantage to having a specialized language or DSL in a larger langage, like C++ or Python, is that one may choose the right tool for the right job without leaving their preferred environment.

In the host language, in this case C++, expressions (values) in `aperture` may be lifted to values in C++, e.g., an expression repesenting a number may be lifted to, say, double, or a lamba expression in `aperture` may be lifted to a standard function (function object) in C++.

```cpp
template <typename T>
T lift(aperature e)
{
    if (is_type<T>(e))
        return value(e);
}
```

If the lift is attempted on an aperature which has free variables, it may make sense if the type `T` itself removes the ambiguity. For instance, what if `T` is a singleton set? Then, there is only one possible value any free variable can have.


## Algebraic Environments (Closures)

Environments ground expressions, i.e., provide free variables with a semantics. We can change the semantics of an expression by changing the environment. For instance, an expression may have a meaningful evaluation whether a particular set of symbols are binded to integer values or matrix values.

Because the aperture is a partial program specification, we may think of the environment as a means of abstraction, i.e., a means of specifying a program in terms of symbols that are not yet bound to values.

### Means of Combination
Environments, as first-class citizens in `aperture`, may be thought of as a binary relation of type `(symbol,exp)`. As a binary relation, we may apply set-theoretic operations on them:

#### Union
Given two environments, we can take the union of them. This operation, however, is not closed over the set of environments, i.e., the union of two environments may not be an environment.

The issue with union is that it may not be well defined. For instance, if we have two environments which contain the same symbol but with differing definitions (vales), then the union of these environments yields a relation that is not a function and the meaning of an environment is ambiguous.

An ambiguous environment kind of defeats the purpose of having an environment, since the environment is the means by which we ground free variables. If the environment is ambiguous, then the free variables are not grounded, and the expression is an aperture. We may consider, then, that the union of two environments in general yields an object that, when an expression is evaluated in it, yields an aperture.

As an aperture, we can then defer the evaluation of the expression to some other context, where the expression is evaluated in a well defined environment. For instance, we may think of the multiple possible values that a symbol may have as a probability distribution, and thus when we evaluate the expression in that "distribution" environment, we get a distribution over the possible evaluations of the expression.

Suppose we have two environments, `E1` and `E2`, where

- `E1 = {x: 1, y: 2}`
- `E2 = {y: 3, z: 4}`

Then we have:

- `E1 ∪ E2 = {x: 1, y: [2, 3], z: 4}`

#### Intersection
Given two environments, we can take the intersection of them. 
Environments are closed under this operation, but may produce surprising results. Suppose we have two environments, `E1` and `E2`, where

- `E1 = {x: 1, y: 2}`
- `E2 = {y: 3, z: 4}`

Then we have:

- `E1 ∩ E2 = {}`

An an aside, subset, equality, and other predicates are straightforward.

## Means of Abstraction

The means of abstraction are of a different kind than the means of combinations.

### Substitution

Given an environment and an expression, we can substitute the symbols in the expression with their bindings in the environment.

This is the most important operation an environment provides. Binding symbols to values.

### Language Models

Language models, in particular Large Language Models (LLMs), provide a new kind of opportunity for making sense of apertures.

If there is a hole in an expression, the context itself may be enough to close the hole.

#### Example

...