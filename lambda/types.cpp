#include "types.h"

using std::string;
using std::ostream;

string type(var const &) { return "var"; }

value eval(var const & e, closure const & c)
{
    if (!c(e.value))
        return null_value{};

    return eval(*c(e.value),c);
}

string type(str const &) { return "str"; }

str eval(str const & e, closure const & c) { return e; }

str operator+(str const & l, str const & r) { return str{l.value+r.value}; }

ostream & operator<<(ostream & out, str const & e) { out << "\"" << e.value << "\""; return out; }

string type(num const &) { return "num"; }

num eval(num const & e, closure const & c) { return e; }

num operator+(num const & l, num const & r) { return num{l.value+r.value}; }

num operator*(num const & l, num const & r) { return num{l.value*r.value}; }

ostream & operator<<(ostream & out, num const & e) { out << e.value; return out; }

