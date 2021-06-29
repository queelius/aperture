#pragma once

/**
 * Provides value semantics for aperture expressions.
 */

#include <memory>
#include "detail/exp.hpp"

namespace aperture
{
    class aperture
    {
    public:
        aperture(detail::exp * e) : e(e) {}
        aperture(aperture const & e) : e(e.e->clone()) {}

        template <typename T>
        auto as() const
        {
            return is_type<T*>(e) ?
                value<T>(e) : std::optional<T>();
        }

    private:
        unique_ptr<detail::exp> e;
    };

    template <typename I>
    aperture parse(I begin, I end)
    {
        // ...
    }

    template <typename I>
    aperture eval(aperture const & e)
    {
        // ...
    }

    
}