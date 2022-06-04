#pragma once

#include <iostream>
#include <memory>
#include <optional>
#include "closure.h"
using std::shared_ptr;
using std::ostream;
using std::string;
using std::optional;
using std::move;

/**
 * The Expression concept refers to expressions (e.g., abstract syntax tree).
 * An object type T models Expression if it satisfies the following:
 *     (1) T can be evaluated to an object that models the concept of Value
 *         by overloading eval,
 *             Expression eval(T).
 *     (2) T can be output to an S-expression representation by overloading
 *         the output stream,
 *            ostream & operator<<(ostream &, T).
 *     (3) A type representation of T can be retrieved by overloading type,
 *            string type(T).
 *
 * The expression class defined next is a type-erasure over types that model
 * the Expression concept, which itself models the Expression concept, e.g..,
 * evaluating it results in a value, which is a type-erasure over
 * objects that model the Value concept.
 *
 * The expression applies type-erasure over concrete types that model the
 * Expression concept to facilitate polymorphic value semantics and decoupling,
 * i.e., instead of intrusively requiring types to inherit from an abstract base
 * class, we only require that it provides the three overloads given above.
 */

/**
 * empty models an empty expression.
 */
struct empty_expr
{
    string type() const { return "empty"; }
    ostream & out(ostream & os) const { os << "()"; }
    empty_expr eval(closure const &) const { return *this; }
};

class expression
{
public:
    expression() : expr_(make_shared<expr_model<empty_expr>>(empty_expr{})) {}

    template <typename T>
    expression(T t) : expr_(make_shared<expr_model<T>>(t)) {}

    expression(expression const &) = default;

    template <typename T>
    optional<T> as() const
    {
        auto t = dynamic_cast<expr_model<T> const *>(expr_.get());
        return t ? optional<T>(t->t) : nullopt;
    }
    
    expression eval(closure const & c) const { return expr_->eval(c); }
    ostream & out(ostream & out) const { return expr_->out(out); }
    string type() const { return expr_->type(); }

private:
    struct expr_concept
    {
        virtual string type() const = 0;
        virtual ostream & out(ostream &) const = 0;
        virtual expression eval(closure const &) const = 0;
    };
    shared_ptr<expr_concept const> expr_;

    template <typename T>
    struct expr_model final : expr_concept
    {
        T t;

        expr_model(T t) : t(move(t)) {}
        ostream & out(ostream & os) const { return t.out(os); }
        expression eval(closure const & c) const { return t.eval(c); }
        string type() const { return t.type(); }
    };
};

