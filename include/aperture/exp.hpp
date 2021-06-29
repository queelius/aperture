#pragma once

/**
 * Provides value semantics for aperture expressions.
 * 
 * Aperture expressions may be constructed in three ways.
 * 
 * (1) Construct an exp using the constructor that
 * accepts an input iterator for the source code.
 * 
 * (2) Construct an exp using the JSON serialization
 * of an aperture expression (AST).
 * 
 * (3) In C++, use native functions, like
 *
 *     cons : (exp,exp) -> exp
 * 
 * or
 * 
 *     template <typename F>
 *     make_proc : F -> exp.
 * 
 * The entire Aperture language set is implemented
 * in C++ so that Aperture programs (abstract syntax trees)
 * can be dynamically built with native C++ code,
 * evaluated,
 * 
 *     eval : (exp,env) -> exp,
 * 
 * applied,
 *
 *     apply : (exp,exp,env) -> exp,
 * 
 * and so on. Any expression exp may in turn
 * be serialized to JSON,
 * 
 *     jsonify : exp -> json
 * 
 * or, if possible, converted into a native type,
 * 
 *     template <typename T>
 *     as : exp -> T.
 *    
 * For instance,
 * 
 *     as<proc_fn<X,Y>> : (lambda + proc) -> fn<X,Y>
 * 
 * is a specialization that takes a lambda or proc
 * expression and maps it to a function object.
 */

#include <memory>
#include "detail/exp.hpp"

namespace aperture
{
    class exp
    {
    public:
        exp(detail::exp * e) : e(e) {}
        exp(exp const & e) : e(e.e->clone()) {}
        exp(json const & js) : e(make_unique<exp>(js)) {}

        template <typename I>
        exp(I begin, I end)
        {
            auto [s,f] = detail::tokenize(begin,end);
            e = detail::read(s,f);
        }

        template <typename T>
        auto as() const
        {
            return is_type<T>(e) ?
                value<T>(e) : std::optional<T>();
        }

        auto eval(env x) { return exp(details::eval(e,x)); }
        auto to_json() const { return e->to_json(); }

    private:
        unique_ptr<detail::exp> e;
    };

    auto eval(exp e, env x)
    {
        return e.eval(x);
    }

    template <typename T>
    auto make_prim(T x)
    {
        return make_unique<details::prim<T>>(x);
    }

    auto make_symbol(std::string x)
    {
        return make_unique<details::symbol>(x);
    }

    template <typename F>
    auto make_proc(F f)
    {
        return make_unique<details::proc>(f);
    }

    template <typename F>
    auto apply(exp lhs, exp rhs, env e)
    {
        return true;
    }

    auto cons(exp h, exp t)
    {
        return exp(details::cons(h.e,t.e));
    }

    auto car(exp e)
    {
        return exp(details::car(e.e));
    }

    auto cdr(exp e)
    {
        return exp(details::cdr(e.e));
    }

}