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

