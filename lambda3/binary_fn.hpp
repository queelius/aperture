#pragma once

#include <functional>
#include "expression.hpp"
#include "closure.h"
#include "num.hpp"

using std::string;
using std::ostream;
using std::function;

namespace lam
{
    template <typename X>
    struct op;

    /**
     * The binary operator expression.
     */
    struct binary_op
    {
        expression l;
        expression r;
        string op_type;

        string type() const { return "add"; }

        expression eval(closure const & c)
        {
            auto e1 = l.eval(c);
            auto e2 = r.eval(c);

            if (auto x = e1.as<num>(), y = e2.as<num>(); x && y)
                return op<num>(op_type)(*x,*y);

            if (auto x = e1.as<str>(), y = e2.as<str>(); x && y)
                return op<str>(op_type)(*x,*y);
        }

        ostream & out(ostream & os)
        {
            os << "(" << op_type << " " << e.l << " " << e.r << ")";
            return os;
        }
    };

    template <typename X>
    struct op
    {
        function<X(X,X)> f;

        op(string op)
        {
            if (op == "+")
                f = [](X x,X y) { return x + y; };
            else if (op == "*")
                f = [](X x,X y) { return x * y; };
        }

        expression operator()(X l, X r) const
        {
            return f(*x,*y);
        }
    };
}