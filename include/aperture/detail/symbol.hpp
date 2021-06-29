#pragma once

#include <string>
#include "exp.hpp"

namespace aperture::detail
{
    struct symbol : exp
    {
        std::string value;

        unique_ptr<exp> clone() const { return make_unique<symbol>(value); }
    };
}