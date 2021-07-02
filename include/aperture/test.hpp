#pragma once

#include <string>
#include <map>
#include <sstream>
#include <utility>
#include <regex>

namespace aperture2
{
    template <typename I>
    auto read(I & begin, I end)
    {
        if (begin == end)
            terminate();

        auto tok = *begin++;
        if (tok.str() == "(")
        {
            auto s = new sexp();
            while (begin->str() != ")")
                s = append(read(begin,end),s);
            //++begin;
            return s;
        }

        if (tok.str() == ")")
        {
            std::cerr << "unexpected closing paren\n";
            std::terminate();
        }
        return atom(tok.str());
    }

    template <typename T>
    auto try_cast(std::string const & s)
    {
        std::stringstream ss(s);
        T x;
        return ((ss >> x).fail() || !(ss >> std::ws).eof()) ?
            std::optional<T>() : std::optional<T>(x);
    }

    template <typename I>
    auto tokenize(I begin, I end)
    {
        static auto const r = std::regex("\\w+(?:'\\w+)?|[^\\w\\s]");
        return std::make_pair(std::sregex_iterator(begin,end,r),std::sregex_iterator());
    }

    struct exp
    {
        virtual exp * clone() const = 0;
    };

    struct symbol : exp
    {
        std::string value;

        symbol(std::string value) : value(value) {}

        exp * clone() const override { return new symbol(value); }
    };

    struct integer : exp
    {
        int value;

        integer(int value) : value(value) {}

        exp * clone() const override { return new integer(value); }
    };

    exp * atom(std::string const & tok)
    {
        if (auto res = try_cast<int>(tok); res)
            return new integer(*res);
        return new symbol(tok);
    }

    struct sexp : exp
    {
        exp * head;
        sexp * tail;

        sexp(exp * head, sexp * tail = nullptr) : head(head), tail(tail) {}

        exp * clone() const override
        {
            return new sexp(head->clone(),tail->clone());
        }
    };

    struct proc : exp
    {
        using proc_ptr = (*exp)(sexp*);

        proc(proc_ptr p) : p(p) {}

        proc_ptr p;
    };

    struct env : exp
    {
        std::map<std::string, exp*> values;
        env * parent;

        exp * clone() const
        {
            std::map<std::string, exp*> vs;
            for (auto const [k,e] : values)
                vs[k] = e->clone();
            return new env(vs,parent->clone());
        }

        auto lookup(std::string const & x)
        {
            auto e = find(x);
            if (e == nullptr)
                terminate();
            return e->values[x];
        }

        auto find(std::string const & x)
        {
            if (values.count(x) != 0)
                return this;

            return parent == nullptr ?
                nullptr : parent->find(x);
        }
    };

    struct lambda : exp
    {
        exp * params;
        exp * body;
        env * closure;

        lambda(exp * params, exp * body, env * closure) :
            params(params->clone()), body(body->clone()), closure(closure->clone()) {}
    };

    exp * eval(exp * x, env * e = global_env)
    {
        if (auto v = dynamic_cast<symbol*>(x); v)
            return e->lookup(v->value);

        if (auto i = dynamic_cast<integer*>(x); i)
            return i;

        if (auto p = dynamic_cast<proc*>(x); p)
            return p;

        if (auto l = dynamic_cast<lambda*>(x); l)
            return l;

        if (auto s = dynamic_cast<sexp*>(x); s)
            return eval(apply(eval(car(s),eval(cdr(s)),e)));

        return nullptr;
    }

    exp * apply(lambda * lam, exp * args)
    {
        return eval(lam->body, extend(lam->e, args));
    }

    exp * apply(proc * f, sexp * args)
    {
        return f->p(args);
    }

    auto cdr(sexp * e)
    {
        return e->tail;
    }

    auto car(sexp * e)
    {
        return e->head;
    }

    auto empty(sexp * e)
    {
        return e == nullptr;
    }

    auto length(sexp * e)
    {
        int l = 0;
        while (e)
            { ++l; e = e->tail; }
        return l;
    }

    auto cons(exp * lhs, sexp * rhs)
    {
        return new sexp(lhs,rhs);
    }

    auto last(sexp * s)
    {
        while (s->tail)
           s = s->tail;
        return s;
    }

    auto push_back(exp * lhs, sexp * rhs)
    {
        auto s = new sexp(lhs,nullptr);
        last(rhs)->tail = s;
        return rhs;
    }

    auto append(sexp * lhs, sexp * rhs)
    {
        if (empty(lhs))
            return rhs;

        last(lhs)->tail = rhs;
        return e
    }
}