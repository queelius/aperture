#pragma once

#include <optional>
#include <map>
#include <string>


class expression;

using std::map;
using std::string;
using std::optional;

class closure
{
public:
    closure(closure const * parent = nullptr);

    optional<expression> operator()(string) const;
    expression & operator[](string);

    closure const * parent;

private:
    map<string, expression> db;
};