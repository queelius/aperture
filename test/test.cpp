#include <iostream>
#include "../include/aperture/test.hpp"

using namespace aperture2;

exp * sum_int(sexp * xs)
{
        int res = 0;
        for (auto i : args->value)
            res += dynamic_cast<integer*>(i)->value;
        return new integer(res);
}

env * global_env()
{
    auto e = new env();
    e->values["+"] = new proc(&sum_int)

    return e;
}

env * global_env = standard_env();

int main()
{
    
}