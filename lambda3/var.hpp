#pragma once

#include "expression.hpp"
#include "closure.h"

/**
 * var models the concept of a symbol.
 * 
 * A symbol may either be substituted for some value or operations may operate
 * directly on symbols, e.g., if x is a var, then
 *     deriv(mult_expr{x,x},x) = mult_expr{2,x}.
 */
struct var
{
    expression eval(closure const & c) const
    {
        if (!c(value))
            return empty_expr{};

        return c(value)->eval(c);
    }

    string type() const { return "var"; }

    string value;
};





