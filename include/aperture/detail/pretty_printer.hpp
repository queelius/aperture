#pragma once

#include "aperture.hpp"

namespace aperture::details
{
    struct pretty_printer
    {
        static void visit(exp * e)
        {
            if (is_symbol(e))
                std::cout << value<symbol>(e) << " ";
            if (is_integer(e))
                std::cout << value<integer>(e) << " ";
            if (is_str(e))
                std::cout << value<str>(e) << " ";
            if (is_list(e))
            {
                if (is_empty(e))
                    std::cout << "()";
                else
                {
                    std::cout << "(";
                    while (!is_empty(e))
                    {
                        visit(car(e));
                        e = cdr(e);
                    }
                    std::cout << ")\n";
                }
            }
            if (is_pair(e))
            {
                visit(car(e));
                std::cout << " . ";
                visit(cdr(e));
            }
            if (is_lambda(e))
            {
                std::cout << "(lambda ";
                auto l = dynamic_cast<lambda*>(e);
                visit(l->params);
                visit(l->body);
                std::cout << ")\n";                
            }
            if (is_define(e))
            {
                auto def = dynamic_cast<define*>(e);
                std::cout << "(define " << def->var << " ";
                visit(def->value);
                std::cout << ")\n";
            }
            if (is_proc(e))
                std::cout << "(proc " << value<proc>(e) << ")\n";
        }            
    };

    void print(exp * x)
    {
        walk(x,pretty_printer{});
    }
}