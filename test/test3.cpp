#include <string>
#include <iostream>
#include <map>
#include <sstream>
#include <utility>
#include <regex>
#include <exception>
#include <optional>

namespace aperture
{
    namespace details
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
        };

        struct empty_list : exp
        {
            exp * clone() override { return this; }
        };
        empty_list * NIL = new empty_list;

        struct symbol : exp
        {
            std::string value;

            symbol(std::string value) : value(value) {}

            exp * clone() override { return new symbol(value); }
        };

        bool is_symbol(exp * e)
        {
            return dynamic_cast<symbol*>(e) != nullptr;
        }

        struct define : exp
        {
            std::string var;
            exp * value;

            define(std::string var, exp * value) : var(var), value(value) {};

            exp * clone() { return new define(var,value); }
        };

        struct integer : exp
        {
            int value;

            integer(int value) : value(value) {}

            exp * clone() override { return new integer(value); }
        };

        exp * atom(std::string const & tok)
        {
            if (auto res = try_cast<int>(tok); res)
                return new integer(*res);
            return new symbol(tok);
        }

        struct str : exp
        {
            std::string value;
            str(std::string value) : value(value) {}
            exp * clone() override { return new str(value); }
        };

        struct pair : exp
        {
            pair(exp * lhs, exp * rhs) : lhs(lhs), rhs(rhs) {}
            exp * lhs;
            exp * rhs;

            exp * clone() override { return new pair(lhs->clone(),rhs->clone()); }
        };

        template <typename T>
        auto value(exp * x) { return dynamic_cast<T*>(x)->value; }

        bool is_empty(exp * p)
        {
            return p == NIL;
        }

        bool is_pair(exp * e)
        {
            return dynamic_cast<pair*>(e) != nullptr;
        }

        bool is_list(exp * e)
        {
            if (auto p = dynamic_cast<pair*>(e); p)
                return is_list(p->rhs);
            return is_empty(e);
        }

        exp * cdr(exp * e)
        {
            auto p = dynamic_cast<pair*>(e);
            if (!p)
            {
                std::cerr << "[cdr] expected pair\n";
                std::terminate();
            }
            return p->rhs;
        }

        exp * car(exp * e)
        {
            auto p = dynamic_cast<pair*>(e);
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
            dynamic_cast<pair*>(last(lhs))->rhs = rhs;
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
                return new env(vs,dynamic_cast<env*>(parent->clone()));
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

            lambda(exp * params, exp * body, env * e) :
                params(params), body(body), e(e) {}

            exp * clone() override
            {
                return new lambda(params->clone(),body->clone(),
                    dynamic_cast<env*>(e->clone()));
            }
        };

        bool is_define(exp * e)
        {
            return dynamic_cast<define*>(e) != nullptr;
        }

        bool is_lambda(exp * e)
        {
            return dynamic_cast<lambda*>(e) != nullptr;
        }

        bool is_integer(exp * e)
        {
            return dynamic_cast<integer*>(e) != nullptr;
        }

        bool is_proc(exp * e)
        {
            return dynamic_cast<proc*>(e) != nullptr;
        }

        bool is_str(exp * e)
        {
            return dynamic_cast<str*>(e) != nullptr;
        }

        bool is_self_evaluating(exp * e)
        {
            return is_proc(e) ||
                is_integer(e) ||
                is_str(e) ||
                is_empty(e) ||
                is_lambda(e);
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
            pair * t = dynamic_cast<pair*>(last(ls));
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
                auto lam = dynamic_cast<lambda*>(op);
                return eval(lam->body, extend(lam->e, lam->params, args));
            }

            std::cerr << "[apply] expected proc\n";
            std::terminate();
        }

        template <typename F>
        void walk(exp * parent, exp * e, F f)
        {
            if (is_lambda(e))
            {
                auto lam = dynamic_cast<lambda*>(e);
                f(parent,e);
                walk(lam,lam->params,f);
                walk(lam,lam->body,f);
                walk(lam,lam->e,f);
            }
            else if (is_pair(e))
            {
                auto p = dynamic_cast<pair*>(e);
                f(parent,e);
                walk(p,car(p),f);
                walk(p,cdr(p),f);
            }
            else
            {
                f(parent,e);
            }
        }

        void print(exp * x, int indent=0)
        {
            walk(nullptr,x,[](exp * parent, exp * e)
            {
                if (is_symbol(e))
                    std::cout << "symbol:" << value<symbol>(e) << "\n";
                if (is_integer(e))
                    std::cout << "int:" << value<integer>(e) << "\n";
                if (is_str(e))
                    std::cout << "str:" << value<str>(e) << "\n";
                if (is_pair(e))
                    std::cout << "pair:";
                if (is_lambda(e))
                    std::cout << "lambda:";
                if (is_proc(e))
                    std::cout << "proc:" << value<proc>(e) << "\n";
            });
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


        exp * make_define(exp * x)
        {
            if (!is_list(x))
            {
                std::cerr << "[make_define] expected list\n";
                std::terminate();
            }

            if (!is_symbol(car(x)) || value<symbol>(car(x)) != "define")
            {
                std::cerr << "[make_define] expected 'define'\n";
                std::terminate();
            }

            exp * var = car(cdr(x));
            exp * body = car(cdr(cdr(x)));
            return new define(value<symbol>(var),body);
        }

        exp * make_lambda(exp * x, env * e)
        {
            if (!is_list(x))
            {
                std::cerr << "[make_lambda] expected list\n";
                std::terminate();
            }

            if (!is_symbol(car(x)) || value<symbol>(car(x)) != "lambda")
            {
                std::cerr << "[eval_lambda] expected 'lambda'\n";
                std::terminate();
            }

            exp * params = car(cdr(x));
            exp * body = car(cdr(cdr(x)));
            return new lambda(params,body,e);
        }

        exp * eval(exp * x, env * e)
        {
            if (is_symbol(x))
                return eval(e->lookup(value<symbol>(x)),e);

            if (is_define(x))
            {
                e->values[dynamic_cast<define*>(x)->var] = value<define>(x);
                return value<define>(x);
            }

            if (is_self_evaluating(x))
                return x;

            if (is_list(x))
            {
                if (!is_list(cdr(x)))
                {
                    std::cerr << "[eval] expected list\n";
                    std::terminate();
                }

                exp * op = car(x);
                if (is_pair(op) && is_symbol(car(op)) && value<symbol>(car(op)) == "lambda")
                    op = make_lambda(op,e);
                else if (is_symbol(op) && value<symbol>(op) == "define")
                    return eval(make_define(x),e);
                //else if (is_symbol(op) && value<symbol>(op) == "string")
                //    return eval(make_string(x),e);
                else
                {
                    if (!is_symbol(op))
                    {
                        std::cerr << "[eval] expected symbol\n";
                        std::terminate();
                    }
                    op = eval(op,e);
                }
                return eval(apply(eval(op,e),eval_list(cdr(x),e)),e);
            }

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
                if (is_empty(e))
                    return new integer(res);

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
                if (is_empty(e))
                    return new integer(res);

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
         *     eval : (exp*,env) -> exp*
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
         * as far as it can, performing as many of the defined
         * substitutions as possible.
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

        /*
        struct aperture : exp
        {
            exp * x;

            exp * free()
            {
                return walk(x, [](exp * x) { return is_symbol(x); });
            }
        };

        exp * close(exp * x, exp * symbols, exp * values)
        {
            if (is_empty(x->free()))
            {
                return make_lambda(x,e);
            }
            else
            {
                return make_aperture(x);
            }
        }
        */
    }

    struct env
    {
        env(details::env * e) : e(e) {};
        env(env const & e) : e(dynamic_cast<details::env*>(e.e->clone())) {}

        auto lookup(std::string const & s) { return e->lookup(s); }

        details::env * e;
    };

    // if no free variables after
    //     close(aperture*, free_symbols) 
    // then return a lambda over the free symbols.
    // otherwise, return an apeture.
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
}

int main()
{
    auto global = aperture::standard_env();

    std::string code = "((lambda (x y) (+ x y)) 1 2)";
    auto x = aperture::exp(code.begin(),code.end());
    auto res = aperture::eval(x,global);
    std::cout << "result: " << aperture::details::value<aperture::details::integer>(res.e) << "\n";

    /*
    using namespace aperture::details;        

    env * global_env = standard_env();

    std::string code = "((lambda (x y) (+ x y)) 1 2)";
    //std::string code = "(define f (+ ((lambda (x y) (+ x y)) 11 22) 2))";
    //std::string code = "(define x 10)";
    auto [b,e] = tokenize(code.begin(),code.end());
    exp * res2 = read(b,e);
    std::cout << "\n\n==========================\n\n";
    print(res2);
    std::cout << "\n\n==========================\n\n";
    exp * res3 = eval(res2,global_env);
    print(res3);
    */

    //print(global_env->lookup("x"));
    /*

    //auto A = new symbol("+");
    //eval(new define("x",new integer(100)),global_env);
    eval(new define("z",new integer(20000)),global_env);
    auto body = cons(new symbol("+"), cons(new symbol("x"), cons(new symbol("z"), NIL)));
    auto params = cons(new symbol("x"), cons(new symbol("y"), NIL));
    auto args = cons(new integer(1000), cons(new integer(200), NIL));
    auto A2 = new lambda(params,body,global_env);
    auto B2 = apply(A2,args);
    print(B2);
    //auto A = new symbol("+");
    //auto B = cons(new symbol("+"), cons(new integer(1), cons(new integer(2), NIL)));
    //auto C = cons(new symbol("*"), cons(new integer(3), cons(new integer(4), NIL)));

    //auto xs = cons(A2, cons(B, cons(C, NIL)));
    
    std::cout << "\n\n===================\n";
    //print(xs);
    print(A2);
    std::cout << "\n------------------\n";

    //auto res = eval(xs,global_env);
    //print(res);
    */
}