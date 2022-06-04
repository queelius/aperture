#pragma once

#include <iostream>
#include <string>
#include "expression.hpp"
#include "closure.h"

using std::string;
using std::ostream;

namespace lam
{
    /**
     * str models the concept of a string.
     */
    struct str
    {
        string value;

        string type() const { return "str"; }

        /**
         * Evaluating a str returns a str as a result.
         */
        str eval(closure const &) { return e; }

        ostream & out(ostream & os) { out << "\"" << e.value << "\""; return os; }
    
    };

    str operator+(str const & l, str const & r) { return str{l.value+r.value}; }
}