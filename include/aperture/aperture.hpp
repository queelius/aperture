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
    struct env
    {
        env(details::env * e) : e(e) {};
        env(env const & e) : e(dynamic_cast<details::env*>(e.e->clone())) {}

        auto lookup(std::string const & s) { return e->lookup(s); }

        details::env * e;
    };

    /**
     * Observe that
     *     eval : (exp,env) -> exp.
     * If the expression cannot be completely evaluated, say it
     * has an ungrounded symbol, then eval returns an exp type
     * with free variables. We call this kind of expression an
     * aperture.
     */
    struct exp
    {
        exp(details::exp * e) : e(e) {}
        exp(exp const & e) : e(e.e->clone()) {}

        template <typename I>
        exp(I begin, I end)
        {
            auto [s,f] = details::tokenize(begin,end);
            e = details::read(s,f);
        }

        auto eval(env const & x) { return exp(details::eval(e,x.e)); }

        details::exp * e;
    };

    auto eval(exp e, env x)
    {
        return e.eval(x);
    }

    auto make_symbol(std::string x)
    {
        return exp(new details::symbol(x));
    }

    /**
     * apply: apply arguments to an operation,
     * e.g., let
     *
     *     E := '((lambda (x y) (+ x y)) (+ 1 2) 3)
     *
     * then
     *
     *     (car E) := (lambda (x y) (+ x y))
     *
     * and
     *
     *     (cdr E) := ((+ 1 2) 3).
     *
     * When we eval an apply expression, we
     * substitute the terms in the rhs
     * with the lambda parameters, i.e.,
     */
    auto apply(exp lhs, exp rhs)
    {
        return exp(details::apply(lhs.e,rhs.e));
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

    auto standard_env()
    {
        return env(details::standard_env());
    }

    template <typename F>
    auto make_proc(F f)
    {
        return exp(new details::proc(f));
    }

    auto global_env = standard_env();

    template <typename X, typename Y>
    struct proc_fn
    {
        proc_fn(details::exp * p) : p(p) {}

        auto operator()(X x)
        {
            return dynamic_cast<Y>(details::apply(p,x));
        }

        details::exp * p;
    };

    template <typename X, typename Y>
    auto lift(details::exp * f)
    {
        return proc_fn<X,Y>(f);
    }

    /*
    exp close(exp x, exp vars)
    {
        //...
    }
    */
}
