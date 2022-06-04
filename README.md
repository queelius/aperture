# Aperture

When mathematicans look at papers containing math symbols,
they have some notion about what the symbols mean.
In this case, their mental model is providing the binding, the
*closure*, for these symbols. We often say that these symbols
are *grounded*.

If the mathetician does not know what some of the symbols mean,
then these symbols are free variables, in which case the
mathematical expression does not have an unambiguous meaning.

When we evaluate an expression, we normally get a particular kind of expression
known as a value, which is a self-evaluating expression. For instance,
`(sum 1 2 3)` is an expression that evaluates to `(number 6)`. However,
`(number 6)` is an expression that evaluates to `(number 6)`, i.e., it is a value.
There are many kinds of values in most programming languages, e.g., numbers
and Booleans.

When you evaluate an expression and the result is a value, then
we have a closure, a complete specification of the program. When we
evaluate an expression and the result is a non-value expression,
then we have what we call an *aperture*, which is an incomplete program
specification.

An aperture is "an opening, hole, or gap." So, if we have an aperature,
there are *holes* in the program's specification that can be filled in
at some other time, i.e., we can pass an aperture around, which is a
partial program specification, and evaluate it in some other context
where either the expression evaluates to a value, or another
aperture.

For insance, suppose we have the expression
`(lambda x (* (+ x y) (+ x y)))`.

If we evaluate this expression, we get an aperature,
`(aperature (lambda x (* (+ x y) (+ x y))))`.

What can we do with this? We could force it to the closure
`(lambda x y (* (+ x y) (+ x y)))`, when evaluates to itself,
and when we evaluate `(apply (lambda x y (* (+ x y) (+ x y))) 1 2)`
we get the value `(number 9)`.

However, we could also evaluate it in some other context, e.g.,
by letting all free variables denote unknown values, and then
when we evaluate it, we get the value
`(lambda x (* (+ x (symbol y)) (+ x (symbol y)))`.
When we evaluate `(apply 
`(apply (lambda x (* (+ x (symbol y)) (+ x (symbol y))) 1)`
we get the value
`(* (+ 1 (symbol y)) (+ 1 (symbol y)))`.

We may then, say, evaluate the expression
`(expand (* (+ 1 (symbol y)) (+ 1 (symbol y))))`,
which obtains the value `(+ 1 (* 2 y) (* y y))`.
we could do other things, like
`(solve(* (+ 1 (symbol y)) (+ 1 (symbol y)), y, 0)`,
which finds the set of values for `y` such that
`(1+y)^2 = 0`.

We see that the aperature is just a partial program specification,
where any subexpression in the aperture is reduced to its most
complete specification (values whenever possible) and any non-value
subexpressions are aperature expressions.

Note that when we evaluate `(aperature e)`, we get the result `(aperature e)`
when evaluated in the same environment as before.




# Generic programming
Programs can work on expressions, including aperture expressions, so we may also
think of apertures as a program template "to be filled in" at some later point.

Apertures, thus, may be seen as a partial program whose complete
specification is a function of the free variables.

One way to bind a free variable in an aperture is to pass it an
additional environment that bounds it to something.
Such environments can either be explicitly passed around as
first-class citizens, or more likely, the current environment may
be extended with a "let" statement.

For instance, suppose E is an aperture expression with x and y
being free variables. Then,

    (let ((x 1) (y 2)) (E))

is a closure. If, instead, we close only x,

    (let ((x 1)) (E))

then the result is another aperture expression with a free variable y.

