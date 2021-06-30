#pragma once
#include "proc.hpp"
#include "exp.hpp"
#include <string>
namespace aperture::detail
{
    struct env : exp
    {
        std::map<std::string, exp*> values;
        env * parent;

        exp * clone() const
        {
            std::map<std::string, exp*> vs;
            for (auto const [k,e] : values)
                vs[k] = e->clone();
            return make_unique<env>(vs,parent->clone());
        }

        auto lookup(std::string const & x)
        {
            auto e = find(x);
            return e == nullptr ?
                unbounded(x) : e->values[x];
        }

        auto find(std::string const & x)
        {
            if (values.count(x) != 0)
                return this;

            return parent == nullptr ?
                nullptr : parent->find(x);
        }
    };
}