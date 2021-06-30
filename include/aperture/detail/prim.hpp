#include "exp.hpp"
#include <string>

namespace aperture::detail
{
    struct integer : exp
    {
        int value;

        exp * clone() const
        {
            return new integer(value);
        }

        O & output(output_iterator<std::string> & o)
        {
            *o = value;
            return ++o;
        }
    };
}