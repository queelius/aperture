
#pragma once
#include "value.h"
#include "expression.h"
#include "types.h"
#include "closure.h"

struct mult_expr
{
    expression l;
    expression r;
};

 string type(mult_expr) { return "mult_expr"; }

 result operator*(result l, result r)
{
    if (type(l) == "num" && type(r) == "num")
        return *l.as<num>() * *r.as<num>();

    return result::null{};
}

/**
 * Evaluating an add expression maps to a type-erased value that models the
 * concept of a result. In fact, we return the result type-erasure. The eval
 * for add_expr must implement logic for every combination of expressions.
 * 
 * We provide overloads for some of these values.
 */
result eval(mult_expr e, closure const & c)
{
    return eval(e.l,c)*eval(e.r,c);
}

/**
 * The output should generate an S-expression representation of the
 * expression.
 */
ostream & operator<<(ostream & out, mult_expr const & e)
{
    out << "(* " << e.l << " " << e.r << ")";
    return out;
}
