#include "addition.h"

 string type(add_expr) { return "add_expr"; }

 value operator+(value l, value r)
{
    if (type(l) == "num" && type(r) == "num")
        return *l.as<num>() + *r.as<num>();

    if (type(l) == "str" && type(r) == "str")
        return *l.as<str>() + *r.as<str>();

    return value::null{};
}

value eval(add_expr e, closure const & c)
{
    return eval(e.l,c)+eval(e.r,c);
}

ostream & operator<<(ostream & out, add_expr const & e)
{
    out << "(+ " << e.l << " " << e.r << ")";
    return out;
}
