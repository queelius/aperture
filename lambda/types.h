#pragma once

#include <iostream>
#include <string>
#include "value.hpp"
#include "expression.hpp"
#include "closure.h"

using std::string;
using std::ostream;

/**
 * num models the concept of a number.
 * 
 * It provides overloads such that it models the concepts of expression and
 * result.
 */
struct num { double value; };

/**
 * var models the concept of a symbol.
 * 
 * A symbol may either be substituted for some value or operations may operate
 * directly on symbols, e.g., if x is a var, then
 *     deriv(mult_expr{x,x},x) = mult_expr{2,x}.
 */
struct var { string value; };

/**
 * str models the concept of a string.
 * 
 * It provides overloads such that it models the concepts of expression and
 * result.
 */
struct str { string value; };

string type(var const &);

/**
 * Evaluating a var in closure c returns
 * the eval of the 
 */
value eval(var const &, closure const &);


string type(str const &);

/**
 * Evaluating a str returns a str as a result.
 */
str eval(str const &, closure const &);
str operator+(str const &, str const &);

/**
 * Both the expression concept and the result concept requires this overload
 * to facilitate outputing its value to a stream.
 */
ostream & operator<<(ostream &, str const &);

string type(num);

/**
 * Evaluating a num returns a num as a result.
 */
num eval(num const &, closure const &);
num operator+(num const &, num const &);
num operator*(num const &, num const &);

/**
 * Both the expression concept and the result concept requires this overload
 * to facilitate outputing its value to a stream.
 */
ostream & operator<<(ostream &, num const &);

