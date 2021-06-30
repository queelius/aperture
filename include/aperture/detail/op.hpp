#include "integer.hpp"

namespace exp::details::op
{
    exp * sum_int(sexp * xs)
    {
            int res = 0;
            for (auto i : args->value)
                res += dynamic_cast<integer*>(i)->value;
            return new integer(res);
    }
}