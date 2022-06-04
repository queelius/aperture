#pragma once

#include <string>
#include <cmath>
using std::string;
using std::ostream;

namespace lam
{
    /**
     * num is a number type that models the concept Expression.
     */
    struct num
    {
        double value;

        string type() const { return "num"; }

        /**
         * Both the expression concept and the result concept requires this overload
         * to facilitate outputing its value to a stream.
         */
        ostream & out(ostream & os) const { os << e.value; return os; }

        /**
         * Evaluating a num returns a num as a result.
         */
        num eval(closure const &) { return e; }
    };

    num operator+(num const & l, num const & r) { return num{l.value+r.value}; }
    num operator*(num const & l, num const & r) { return num{l.value*r.value}; }
    num operator-(num const & l, num const & r) { return num{l.value-r.value}; }
    num operator/(num const & l, num const & r) { return num{l.value/r.value}; }
    num sin(num const & theta) { return num{sin(theta.value)}; }
    num cos(num const & theta) { return num{cos(theta.value)}; }
    num log(num const & theta) { return num{log(theta.value)}; }
    num ceil(num const & theta) { return num{ceil(theta.value)}; }
}