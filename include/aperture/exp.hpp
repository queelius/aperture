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
 * (3) Write C++ code, using functions like
 *
 *     cons : (exp,exp) -> exp
 * 
 * or
 * 
 *     template <typename F>
 *     make_proc : F -> exp.
 * 
 * The entire Aperture language set is implemented
 * in this way so that Aperture programs (abstract syntax trees)
 * can be built directly with C++ code, rather than Aperture.
 *
 * To evaluate an Aperature prorgam (AST), the eval function
 * may be called,
 * 
 *     eval : (exp,env) -> exp.
 * 
 * Many evaluations do not change the expression. For instance,
 * 
 *     eval : prim<T> -> prim<T>
 * 
 * and
 * 
 *     eval : (lambda,env) -> lambda.
 * 
 * However,
 * 
 *     eval : (apply<lambda,exp>,env) -> (prim<T>+exp+symbol+aperture).
 * 
 * If we have two expressions, we may try to apply
 * one expression to the other in the designiated environment,
 *
 *     apply : (exp,exp,env) -> exp,
 *     apply : (proc,exp,env) -> exp
 *     apply : (lambda,exp,env) -> exp.
 * 
 * Any expression exp may in turn be serialized to JSON,
 * 
 *     jsonify : exp -> optional<json>
 * 
 * or, if possible, converted into a C++ type,
 * 
 *     template <typename T>
 *     as : exp -> optional<T>.
 *    
 * For instance,
 * 
 *     as<proc_fn<X,Y>> : (lambda + proc) -> optional<fn<X,Y>>
 * 
 * is a specialization that takes a lambda or proc
 * expression and maps it to a function object with
 * an domain X and codomain Y.
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