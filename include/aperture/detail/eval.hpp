#pragma once
#include "exp.hpp"
#include "symbol.hpp"
#include "proc.hpp"
#include "lambda.hpp"
#include "prim.hpp"

namespace aperture::detail
{
    exp * eval(exp * x, env * e = global_env)
    {
        if (auto v = dynamic_cast<symbol*>(x); v)
            return e->lookup(v->value);

        if (auto i = dynamic_cast<prim*>(x); i)
            return i;

        if (auto p = dynamic_cast<proc*>(x); p)
            return p;

        if (auto l = dynamic_cast<lambda*>(x); l)
            return l;

        if (auto s = dynamic_cast<sexp*>(x); s)
            return eval(apply(eval(car(s),eval(cdr(s)),e)));

        return nullptr;
    }
}