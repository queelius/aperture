#pragma once

#include <nlohmann/json.hpp>
#include <memory>

using nlohmann::json;
using std::unique_ptr;
using std::make_unique;

namespace aperture::detail
{
    /**
     * exp : an expression (AST) in aperture.
     */
    struct exp
    {
        virtual json            to_json() = 0;
        virtual unique_ptr<exp> from_json(exp *) = 0;
        virtual unique_ptr<exp> clone() const = 0;

        /**
         * An expression is closed if it has no free variables.
         */
        //virtual bool closed() const = 0;

        //virtual exp * let(sexp *) const = 0;
    };
}