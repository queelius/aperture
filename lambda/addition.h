#pragma once

#include "value.h"
#include "expression.h"
#include "closure.h"

/**
 * The add expression.
 */
struct add_expr
{
    expression l;
    expression r;
};

/**
 * Evaluating it generates a value that models a result type.
 * We provide overloads for some of these values.
 */
 string type(add_expr);

 value operator+(value, value);

/**
 * Evaluating an add expression maps to a type-erased value that models the
 * concept of a result. In fact, we return the result type-erasure. The eval
 * for add_expr must implement logic for every combination of expressions.
 * 
 * We provide overloads for some of these values.
 */
value eval(add_expr, closure const &);

/**
 * The output should generate an S-expression representation of the
 * expression.
 */
ostream & operator<<(ostream &, add_expr const &);
