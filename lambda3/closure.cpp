#include "closure.h"
#include "expression.hpp"

using std::map;
using std::string;
using std::optional;
using std::nullopt;

closure::closure(closure const * parent) : parent(parent) {}

optional<expression> closure::operator()(string x) const
{
    if (db.count(x) != 0)
        return db.at(x);

    if (parent != nullptr)
        return parent->operator()(x);

    return nullopt;
}

expression & closure::operator[](string x)
{
    return db[x];
}
