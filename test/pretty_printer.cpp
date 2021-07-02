    void print(exp * e, int d = 0)
    {
        auto indent = [](int d)
        {
            for (int i = 0; i < d; ++i)
                std::cout << " ";            
        };

        if (auto v = dynamic_cast<lambda*>(e); v)
        {
            indent(d);
            std::cout << "lambda:";
            print(v->params,d);
            std::cout << "\n";
            indent(d);
            print(v->body,d);
            std::cout << "\n";
            indent(d);
            print(v->e,d);
            std::cout << "\n";

            return;
        }

        if (auto v = dynamic_cast<symbol*>(e); v)
        {
            indent(d);
            std::cout << "symbol:" << v->value << "\n";
            return;
        }

        if (auto v = dynamic_cast<str*>(e); v)
        {
            indent(d);
            std::cout << "str:" << v->value << "\n";
            return;
        }

        if (auto v = dynamic_cast<proc*>(e); v)
        {
            indent(d);
            std::cout << "proc:" << &v->value << "\n";
            return;
        }

        if (auto i = dynamic_cast<integer*>(e); i)
        {
            indent(d);
            std::cout << "integer:" << i->value << "\n";
            return;
        }

        if (auto n = dynamic_cast<empty_list*>(e); n)
        {
            indent(d);
            std::cout << "nil" << "\n";
            return;
        }

        if (auto n = dynamic_cast<env*>(e); n)
        {
            indent(d);
            for (auto [k,expr] : n->values)
            {
                indent(d);
                std::cout << k << " =>\n";
                print(expr,d+2);
            }
            return;
        }


        if (auto p = dynamic_cast<pair*>(e); p)
        {
            indent(d);
            std::cout << "pair (";
            if (is_list(p))
            {
                exp * o = car(p);
                print(o,d+2);
                exp * rest = cdr(p);
                while (true)
                {
                    std::cout << " ";
                    if (auto p = dynamic_cast<empty_list*>(rest); p)
                        break;
                    print(car(rest),d+2);
                    rest = cdr(rest);
                }
            }
            else
            {
                print(car(p),d+2);
                indent(d);
                std::cout << " . ";
                print(cdr(p),d+2);
            }

            indent(d);
            std::cout << ")";
            return;
        }
    }

