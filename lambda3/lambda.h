#pragma once
#include <vector>
#include <iostream>
#include "value.hpp"
#include "expression.hpp"
#include "types.h"
#include "closure.h"

using std::ostream;
using std::vector;
using std::nullopt;
using std::optional;

struct lambda_expr
{
    vector<var> args;
    expression body;
};

struct lambda_value
{
    vector<var> args;
    expression body;
    closure c;

    optional<result> operator()(vector<result> args) const
    {
        if (this->args.size() != args.size())
            return nullopt;

        for (size_t i = 0; i < args.size(); ++i
            c[this->args[i]] = eval(args[i]);

        return eval(body,c);
    }
};

ostream & operator<<(ostream & out, lambda_value const & r)
{
    out << "(lambda (";
    for (auto arg : r.args)
        out << arg << " ";
    out << ") " << body << "\n";

}

 string type(lambda_expr) { return "lambda_expr"; }

/**
 * Eval maps a lambda expression to an anonymous function, denoted lambda_result,
 * of the same form. It accepts a vector of results as arguments and returns a
 * result if the lambda applied to the arguments is well-defined.
 * 
 * It's well defined if, when evaluating the body with the arguments substituted
 * into it, e.g., given the lambda expression
 *     f := (lambda (var x var y var z) (+ (* var x var x) var y var q)
 * and a closure c, eval(f,c) maps to the evaluation of the expression
 *     f(x,y,z) := eval(add_expr{add_expr{add_expr{mult_expr var x var x} y} c(q)},c),
 * which is a lambda_result.
 * 
 * lambda_result is well-defined if a tuple of three values are passed to
 * it (where x is the first argument, y is the second, and z is the third) such
 * that the 
 * 
 * If x and y arand z is a arbitrary type, then this will work. if x and y are strings, then
 * it won't work since * is not defined.
 */
lambda_value eval(lambda_expr e, closure c)
{
    return lambda_value{e.args,e.body,c};
}

/**
 * The output should generate an S-expression representation of the
 * lambda expression, (lambda (x1 x2 ... xn) (f x1 x2 ... xn))
 */
ostream & operator<<(ostream & out, lambda_expr const & e)
{
    out << "(lambda " << e.args << " " << e.body << ")";
    return out;
}
