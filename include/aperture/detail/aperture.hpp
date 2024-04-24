#pragma once

#include <string>
#include <iostream>
#include <map>
#include <sstream>
#include <utility>
#include <regex>
#include <exception>
#include <optional>

namespace aperture::details
{
    template <typename T, >
    auto try_cast(std::string const & s)
    {
        std::stringstream ss(s);
        T x;
        return ((ss >> x).fail() || !(ss >> std::ws).eof()) ?
            std::optional<T>() : std::optional<T>(x);
    }

    struct exp
    {
        virtual exp * clone() = 0;
        virtual bool is_equal(exp *) = 0;
        virtual ~exp() = 0;
    };

    template <typename T>
    T * to(exp * e)
    {
        return dynamic_cast<T*>(e);
    }

    template <typename T>
    auto value(exp * x)
    {
        return to<T>(x) == nullptr ? nullptr : to<T>(x)->value;
    }

    // atom: NIL
    struct nil : exp
    {
        exp * clone() override { return this; }
    };
    exp * NIL = new nil;

    // atom: symbol
    struct symbol : exp
    {
        std::string value;

        symbol(std::string value) : value(value) {}

        exp * clone() override { return new symbol(value); }
    };

    bool is_nil(exp * e)
    {
        return e == NIL;
    }

    bool is_symbol(exp * e)
    {
        return to<symbol>(e) != nullptr;
    }

    struct integer : exp
    {
        int value;

        integer(int value) : value(value) {}

        exp * clone() override { return new integer(value); }
    };

    struct str : exp
    {
        std::string value;
        str(std::string value) : value(value) {}
        exp * clone() override { return new str(value); }
    };

    exp * s(std::string x) { return new str(x); }

    struct boolean : exp
    {
        bool value;
        boolean(bool value) : value(value) {}
        exp * clone() override { return new boolean(value); }
    }
    auto T = new boolean(true);
    auto F = new boolean(false);

    struct pair : exp
    {
        pair(exp * left, exp * right) : left(left), right(right) {}
        exp * left;
        exp * right;

        ~exp() { delete left; delete right; }
        exp * clone() override { return cons(left->clone(),right->clone()); }
    };

    exp * atom(std::string const & tok)
    {
        if (tok == "#t") return T;
        if (tok == "#f") return F;
        if (auto res = try_cast<int>(tok); res)
            return new integer(*res);
        return new symbol(tok);
    }

    bool is_atom(exp * e) { return !is_pair(e); }

    bool is_boolean(exp * e) { return to<boolean>(e) != nullptr; }

    bool is_list(exp * e) { return is_pair(e) ? is_list(cdr(p)) : is_nil(e); }

    /**
     * We do not use reduce for and/or so that we can enable
     * short-circuiting.
     */
    exp * boolean_and(exp * xs)
    {
        if (is_nil(xs))         return T;
        if (is_atom(xs))        return xs == T;
        return car(xs) == F ?   F : boolean_and(cdr(xs));
    }

    exp * boolean_or(exp * xs)
    {
        if (is_nil(xs))     return F;
        if (is_atom(xs))    return xs == T;
        return car(xs) == T ? T : boolean_or(cdr(xs));
    }

    // F models a binary operator of type (exp*,exp*) -> exp*
    template <typename F>
    exp * reduce(exp * xs, exp * i, F f)
    {
        if (is_nil(xs)) return i;
        if (is_atom(xs)) return xs;
        if (is_pair(xs)) return f(car(xs),reduce(cdr(xs)));
        return err(cons(lhs,rhs),"reduce: expected a pair or an atom");
    }

    template <typename T>
    exp * plus(exp * xs)
    {
        return reduce(xs,new prim<T>(0),[](exp* l, exp* r)
            { return new prim<T>(value<prim<T>>(l) + value<prim<T>>(r)); });
    }

    


    /**
     * errors are first-class citizens. when you eval an error, it terminates
     * with error message.
     */
    exp * err(exp * e, exp * msg)
    {
        return list(s("err"),e,msg);
    }

    exp * cdr(exp * e)
    {
        return is_pair(e) ? to<pair>(e)->right : err(e,s("cdr expected pair"));
    }

    exp * car(exp * e)
    {
        return is_pair(e) ? to<pair>(e)->left : err(e,s("car expected pair"));
    }

    exp * cons(exp * left, exp * right) { return new pair(left,right); }

    exp * last_pair(exp * e)
    {
        if (!is_pair(e))
            return err(e,s("last_pair expected pair"));

        while (is_pair(cdr(e))) e = cdr(e);
        return e;
    }   

    exp * append(exp * left, exp * right)
    {
        if (!is_list(left))
            return err(cons(left,right),s("append expected left to be a list"));
        
        to<pair>(last_pair(left))->right = right;
        return left;
    }

    struct proc : exp
    {
        typedef exp*(*proc_t)(exp*);

        proc(proc_t value) : value(value) {}

        exp * clone() override { return new proc(value); }

        exp * apply(exp * args) { return value(args); }

        proc_t value;
    };

    struct env : exp
    {
        std::map<std::string, exp*> values;
        env * parent;

        env(env * parent = nullptr) : parent(parent) {}

        env(std::map<std::string, exp*> values, env * parent = nullptr) :
            values(values), parent(parent) {}

        exp * clone() override
        {
            std::map<std::string, exp*> vs;
            for (auto const [k,e] : values)
                vs[k] = e->clone();
            return new env(vs,to<env>(parent->clone()));
        }

        exp * lookup(std::string const & x)
        {
            if (values.count(x) != 0)
                return values.at(x);

            return parent == nullptr ?
                error(new symbol(x), s("lookup undefined symbol")) : parent->lookup(x);
        }

        env * find(std::string const & x)
        {
            if (values.count(x) != 0)
                return this;

            return parent == nullptr ?
                nullptr : parent->find(x);
        }
    };

        // (cons 'env ( cons parent ( env_list )))
        // (cons env ( cons parent env_list ))
        // where
        //     PARENT (pair*) denotes the parent environent
        // and
        //     ENV_LIST denotes a list of pairs
        //         ( (str* . exp*), ..., (str* . exp*) )
        //     that describes the top-level environment.
        //
        // when we eval(environment), we get an env* primitive. at this point,
        // it is treated as an atomic object that may be queried and is
        // implemented for efficiency. we could 

    bool is_env(exp * e)
    {
        return to<env>(e) != nullptr;
    }

    env * extend(env * e, exp * params, exp * args)
    {
        env * closure = new env(e);
        if (!is_list(params) || !is_list(args))
        {
            std::cerr << "[extend] expected param/args to be a list\n";
            std::terminate();
        }

        // check for same size

        while (!is_empty(params))
        {
            exp * s = car(params);
            if (!is_symbol(s))
            {
                std::cerr << "[extend] expected symbol\n";
                std::terminate();
            }

            closure->values[value<symbol>(s)] = car(args);
            args = cdr(args);
            params = cdr(params);
        }
        return closure;
    }

    exp * eval(exp *, env *);

    struct lambda : exp
    {
        exp * params;
        exp * body;
        env * e;

        bool is_equal(exp * rhs) override
        {
            auto l = to<lambda>(rhs);
            return l != nullptr &&
                   params->is_equal(l->params) &&
                   body->is_equal(l->body) &&
                   e->is_equal(l->e);
        }

        lambda(exp * params, exp * body, env * e) :
            params(params, body, e(e) {}

        ~lambda() { delete params; delete body; delete e; }

        exp * apply(exp * args)
        {
            return eval(body, extend(e, params, args));
        }

        exp * clone() override
        {
            new lambda(params->clone(),body->clone(),to<env>(e->clone()));
        }
    };

    bool is_integer(exp * e)
    {
        return to<integer>(e) != nullptr;
    }

    bool is_proc(exp * e)
    {
        return to<proc>(e) != nullptr || to<lambda>(e) != nullptr;
    }

    bool is_str(exp * e)
    {
        return to<str>(e) != nullptr;
    }

    bool is_self_evaluating(exp * e)
    {
        return is_proc(e) ||
            is_integer(e) ||
            is_str(e) ||
            is_nil(e) ||
            is_boolean(e);
    }

    exp * apply(exp * op, exp * args)
    {
        if (!is_pair(args))
            return err(args,"apply expected args to be a pair");

        if (is_proc(op))
            return value<proc>(op)->apply(args);

        return err(op, s("apply expected proc");
    }

    template <typename F>
    void walk(exp * e, F f)
    {
        if (is_pair(e))
        {
            auto p = to<pair>(e);
            f.visit(e);
            walk(car(p),f);
            walk(cdr(p),f);
        }
        else
        {
            f.visit(e);
        }
    }

    template <typename F>
    bool pair_walk(exp * left, exp * right, F f)
    {
        if (is_pair(left))
        {
            if (!is_pair(right))
                return false;

            f.visit(left,right);
            pair_walk(car(left),car(right),f);
            pair_walk(cdr(left),cdr(right),f);
        }
        else
        {
            f.visit(left,right);
        }
    }

    exp * eval_pair(exp * xs, env * e)
    {
        return is_pair(xs) ?
            cons(eval(car(xs),e),eval_pair(cdr(xs),e)) : eval(xs,e);
    }

    exp * make_list(exp* ... args)
    {
        va_list es;
        va_start(es, args);

        va_arg(es,exp*);

        va_end(es);
    }

    exp * make_define(symbol * x, exp * value, env * e)
    {
        e->values[x->value] = value;
        return value;
    }

    exp * eval(exp * x, env * e)
    {
        if (is_self_evaluating(x))
            return x;

        if (is_symbol(x))
            return eval(e->lookup(value<symbol>(x)),e);

        // must be a pair past this point
        if (is_symbol(car(l)) && value<symbol>(car(l)) == "define")
            return make_define(car(cdr(l)), cdr(cdr(l)), e);

        if (is_symbol(car(l)) && value<symbol>(car(l)) == "lambda")
            return make_lambda(car(cdr(l)), car(cdr(cdr(l))),e);

        /**
         * application
         */
        if (is_pair(x))
            return apply(eval(car(x),e),eval_pair(cdr(x),e));

        return err(x, s("expected self-evaluating expression, symbol, or pair"));
    }

    env * standard_env()
    {
        env * e = new env;
        e->values["+"] = new proc(&plus<number>);
        e->values["*"] = new proc(&prod<number>);
        return e;
    }

    // parsers
    template <typename I>
    exp * read(I & begin, I end)
    {
        if (begin == end)
            return err(nullptr, s("read encountered unexpected EOF"));

        auto tok = *begin;
        ++begin;
        if (tok.str() == "(")
        {
            exp * l = NIL;
            while (begin->str() != ")")
                l = push_back(l,read(begin,end));
            ++begin;
            return l;
        }
        
        if (tok.str() == ")")
            return err(nullptr, s("read encountered unexpected ')'"));

        return atom(tok.str());
    }

    template <typename I>
    auto tokenize(I begin, I end)
    {
        static auto const r = std::regex("\\w+(?:'\\w+)?|[^\\w\\s]");
        return std::make_pair(std::sregex_iterator(begin,end,r),std::sregex_iterator());
    }

    /**
     * When you apply
     *
     *     eval : (exp*,env*) -> exp*
     * 
     * and at least one symbol is undefined,
     * then the result is an aperture.
     * 
     * Essentially, an aperture has free variables.
     * If a closed computation is desired, then
     * 
     *     is_closed : exp* -> bool
     * 
     * where
     * 
     *     is_closed(x) := is_empty(free(x))
     * 
     * may be used to test for that result.
     * 
     * When we evaulate an expression, even though an
     * aperture may be returned, the evaluation still proceeds
     * whereever it can, so some branches of the evaluation may
     * return apertures and others may return non-apetures, like
     * integers or lambdas.
     * 
     * A free variable is a symbol, but the symbol can denote
     * any thing, like a procedure, integer, and so on.
     * 
     * One may take an aperture, and apply a different, decoupled set
     * of rewrite rules. You may imagine a pipeline, where an aperture
     * is passed through multiple-stages of rewrites, filling in the holes
     * as it becomes possible to do so.
     * 
     * The moment of binding has historically been any of the following:
     * 
     *     (1) scanner time (lexemes)
     *     (2) parser time (mapping the lexemes to, say, an AST)
     *     (3) evaluation time
     * 
     * Now we see that even when evaluated, bindings may still be
     * unavaialable. Some dispatching mechanism may be applied to look
     * for a binding. Suppose it is an algebraic expression where the
     * non-binded symbols represent unknowns. Then, an example of
     * a special evaluator is an algebraic solver.
     */

    exp * free(exp * e)
    {
        exp * es = NIL;
        walk(e, [&es](exp * x) { if (is_symbol(x)) es = cons(x,es); });
        return es;
    }

    bool is_equal(exp * lhs, exp * rhs)
    {
        if (lhs == rhs)
            return true;

        return pair_walk(lhs,rhs,[](exp * l, exp * r)
        {
            
        });

        if (is_pair(lhs) && is_pair(rhs))
            return is_equal(car(lhs),car(rhs)) &&
                   is_equal(cdr(lhs),cdr(rhs));
        if (is_lambda(lhs) && is_lambda(rhs))
            return is_equal(lambda_body(lhs),
                   subst(lambda_body(rhs->clone()),
                         lambda_params(rhs), lambda_params(lhs)));
        if (is_integer(lhs) && is_integer(rhs))
            return value<integer>(lhs) == value<integer>(rhs);
        if (is_symbol(lhs) && is_symbol(rhs))
            return value<symbol>(lhs) == value<symbol>(rhs);
        if (is_env(lhs) && is_env(rhs))
            return to<env>(lhs)->values == to<env>(rhs)->values &&
                   is_equal(parent(lhs),parent(rhs));
        if (is_str(lhs) && is_str(rhs))
            return value<str>(lhs) == value<str>(rhs);

        return false;
    }

    exp * subst(exp * x, exp * rep, exp * sub)
    {
        walk(x, [](exp * e)
        {
            auto r = rep;
            auto s = sub;
            while (!is_empty(r))
            {
                if (is_equal(car(r),e))
                {
                    e = car(s)->clone();
                    break;
                }
                r = cdr(r);
                s = cdr(s);
            }
        });
    }

}