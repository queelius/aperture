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
    template <typename T>
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
        virtual ~exp() = 0;
    };

    template <typename T>
    T * to(exp * e)
    {
        return dynamic_cast<T*>(e);
    }

    struct empty_list : exp
    {
        exp * clone() override { return this; }
    };
    exp * NIL = dynamic_cast<exp*>(new empty_list);

    struct symbol : exp
    {
        std::string value;

        symbol(std::string value) : value(value) {}

        exp * clone() override { return new symbol(value); }
    };

    bool is_symbol(exp * e)
    {
        return to<symbol>(e) != nullptr;
    }

    struct define : exp
    {
        std::string var;
        exp * value;

        define(std::string var, exp * value) : var(var), value(value) {}

        ~define() { delete value; }

        exp * clone() { return new define(var,value); }
    };

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

    exp * atom(std::string const & tok)
    {
        if (auto res = try_cast<int>(tok); res)
            return new integer(*res);
        return new symbol(tok);
    }

    struct pair : exp
    {
        pair(exp * lhs, exp * rhs) : lhs(lhs), rhs(rhs) {}
        exp * lhs;
        exp * rhs;

        ~exp() { delete lhs; delete rhs; }
        exp * clone() override { return new pair(lhs->clone(),rhs->clone()); }
    };

    template <typename T>
    auto value(exp * x) { return to<T>(x)->value; }

    bool is_empty(exp * p)
    {
        return p == NIL;
    }

    bool is_pair(exp * e)
    {
        return to<pair>(e) != nullptr;
    }

    bool is_list(exp * e)
    {
        if (auto p = to<pair>(e); p)
            return is_list(p->rhs);
        return is_empty(e);
    }

    exp * cdr(exp * e)
    {
        auto p = to<pair>(e);
        if (!p)
        {
            std::cerr << "[cdr] expected pair\n";
            std::terminate();
        }
        return p->rhs;
    }

    exp * car(exp * e)
    {
        auto p = to<pair>(e);
        if (!p)
        {
            std::cerr << "[car] expected pair\n";
            std::terminate();
        }
        return p->lhs;
    }

    exp * cons(exp * lhs, exp * rhs)
    {
        return new pair(lhs,rhs);
    }

    exp * last(exp * ls)
    {
        if (!is_list(ls))
        {
            std::cerr << "[last] expected a list\n";
            std::terminate();
        }

        if (is_empty(ls))
        {
            std::cerr << "[last] expected non-empty list\n";
            std::terminate();
        }

        while (!is_empty(cdr(ls)))
            ls = cdr(ls);
        return ls;
    }   

    exp * append(exp * lhs, exp * rhs)
    {
        to<pair>(last(lhs))->rhs = rhs;
        return lhs;
    }

    struct proc : exp
    {
        typedef exp*(*proc_t)(exp*);

        proc(proc_t value) : value(value) {}

        exp * clone() override { return new proc(value); }

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

            if (parent == nullptr)
            {
                std::cerr << "[lookup] undefined symbol: " << x << "\n";
                std::terminate();
            }

            return parent->lookup(x);
        }

        env * find(std::string const & x)
        {
            if (values.count(x) != 0)
                return this;

            return parent == nullptr ?
                nullptr : parent->find(x);
        }
    };

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

    struct lambda : exp
    {
        exp * params;
        exp * body;
        env * e;

        lambda(exp * params, exp * body) :
            params(params->clone()), body(body->clone()), e(nullptr) {}

        ~lambda() { delete params; delete body; delete e; }

        exp * clone() override
        {
            auto copy = new lambda(params->clone(),body->clone());
            if (e != nullptr)
                copy->e = to<env>(e->clone());
            return copy;
        }
    };

    exp * lambda_body(exp * l)
    {
        return to<lambda>(l)->body;
    }

    exp * lambda_params(exp * l)
    {
        return to<lambda>(l)->params;
    }

    exp * lambda_env(exp * l)
    {
        return to<lambda>(l)->env;
    }

    bool is_define(exp * e)
    {
        return to<define>(e) != nullptr;
    }

    bool is_lambda(exp * e)
    {
        return to<lambda>(e) != nullptr;
    }

    bool is_integer(exp * e)
    {
        return to<integer>(e) != nullptr;
    }

    bool is_proc(exp * e)
    {
        return to<proc>(e) != nullptr;
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
            is_empty(e);
    }

    exp * push_back(exp * ls, exp * x)
    {
        if (!is_list(ls))
        {
            std::cerr << "[last] expected a list\n";
            std::terminate();
        }

        if (is_empty(ls))
            return new pair(x,NIL);
        pair * t = to<pair>(last(ls));
        t->rhs = new pair(x,NIL);
        return ls;
    }

    exp * eval(exp *, env *);

    exp * apply(exp * op, exp * args)
    {
        if (!is_list(args))
        {
            std::cerr << "[apply] expected argument list\n";
            std::terminate();
        }

        if (is_proc(op))
        {
            return value<proc>(op)(args);
        }

        if (is_lambda(op))
        {
            auto lam = to<lambda>(op);
            return eval(lam->body, extend(lam->e, lam->params, args));
        }

        std::cerr << "[apply] expected proc\n";
        std::terminate();
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


    exp * eval_list(exp * xs, env * e)
    {
        exp * rs = NIL;
        while (!is_empty(xs))
        {
            exp * res = eval(car(xs),e);
            rs = push_back(rs,res);
            xs = cdr(xs);
        }
        return rs;
    }

    exp * make_str(exp * x)
    {
        // (str "test")
        return nullptr;
    }

    exp * make_define(exp * x)
    {
        if (!is_list(x))
        {
            std::cerr << "[make_define] expected list\n";
            std::terminate();
        }

        return new define(value<symbol>(car(x)),car(cdr(x)));
    }

    exp * make_lambda(exp * x)
    {
        if (!is_list(x))
        {
            std::cerr << "[make_lambda] expected list\n";
            std::terminate();
        }

        // parameter list
        if (!is_list(car(x)))
        {
            std::cerr << "[make_lambda] expected parameter list\n";
            std::terminate();
        }

        // body
        if (is_empty(cdr(x)))
        {
            std::cerr << "[make_lambda] expected non-empty body\n";
            std::terminate();
        }

        return new lambda(car(x),car(cdr(x)));
    }

    exp * eval(exp * x, env * e)
    {
        if (is_symbol(x))
            return eval(e->lookup(value<symbol>(x)),e);

        if (is_define(x))
        {
            e->values[to<define>(x)->var] = value<define>(x);
            return value<define>(x);
        }

        if (is_lambda(x))
        {
            to<lambda>(x)->e = e;
            return x;
        }

        if (is_self_evaluating(x))
            return x;

        /**
         * application
         */
        if (is_list(x))
            return eval(apply(eval(eval(car(x),e),e),eval_list(cdr(x),e)),e);

        std::cerr << "[eval] expected self-evaluating expression, symbol, or list\n";
        std::terminate();
    }

    env * standard_env()
    {
        env * e = new env;
        e->values["+"] = new proc([](exp * e) -> exp*
        {
            if (!is_list(e))
            {
                std::cerr << "[proc:+] expected a list\n";
                std::terminate();
            }
            int res = 0;
            while (!is_empty(e))
            {
                res += value<integer>(car(e));
                e = cdr(e);
            }
            return new integer(res);
        });

        e->values["*"] = new proc([](exp * e) -> exp*
        {
            if (!is_list(e))
            {
                std::cerr << "[proc:*] expected a list\n";
                std::terminate();
            }

            int res = 1;
            while (!is_empty(e))
            {
                res *= value<integer>(car(e));
                e = cdr(e);
            }
            return new integer(res);
        });
        return e;
    }

    // parsers
    template <typename I>
    exp * read(I & begin, I end)
    {
        if (begin == end)
        {
            std::cerr << "[read] unexpected EOF\n";
            std::terminate();
        }

        auto tok = *begin;
        ++begin;
        if (tok.str() == "(")
        {
            exp * l = NIL;
            while (begin->str() != ")")
                l = push_back(l,read(begin,end));
            ++begin;

            if (is_symbol(car(l)) && value<symbol>(car(l)) == "define")
                return make_define(cdr(l));

            if (is_symbol(car(l)) && value<symbol>(car(l)) == "lambda")
                return make_lambda(cdr(l));
            
            return l;
        }
        
        if (tok.str() == ")")
        {
            std::cerr << "[read] unexpected closing paren\n";
            std::terminate();
        }

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