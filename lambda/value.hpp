#pragma once

#include <memory>
#include <optional>
using std::shared_ptr;
using std::ostream;
using std::string;
using std::optional;
using std::nullopt;
using std::move;
using std::make_shared;

/**
 * null models a null value.
 * 
 * A null values typically occur due to applying eval to an undefined
 * expression.
 */
struct null_value {};
string type(null_value const &) { return "null_value"; }
ostream & operator<<(ostream & out, null_value const &) { out << "null_value"; return out; }


/**
 * The value concept models the computational value of evaluating an
 * expression, where an expression models the concept of an abstract syntax
 * tree.

 * An object type T models the value concept if it provides function overloads
 * for:
 *     string type(T)
 *     ostream & operator<<(ostream &, T)
 * 
 * By this definition, the value class models the concept of a value.
 * The value class applies type-erasure over concrete types that model the
 * value type to facilitate polymorphic value semantics and decoupling, i.e.,
 * instead of intrusively requiring types to inherit from an abstract base
 * class, we only require that it provides the two overloads given above.
 */
class value
{
private:
    struct value_concept
    {
        virtual string get_type() const = 0;
        virtual ostream & out(ostream &) const = 0;
    };
    shared_ptr<value_concept const> value_;

    template <typename T>
    struct value_model final : value_concept
    {
        T t;

        value_model(T t) : t(move(t)) {}
        ostream & out(ostream & out) const { out << t; return out; }
        string get_type() const { return type(t); }
    };

public:
    /**
     * Default copy constructor.
     * 
     * Since shared_ptr<value_concept const> is const, it has value semantics
     * and thus deep copies are not necessary.
     */
    value(value const &) = default;

    /**
     * By default, we construct a null value.
     */
    value() : value_(make_shared<value_model<null_value>>(null_value{})) {}

    /**
     * Type-erase an object T where T models a value type.
     */
    template <typename T>
    value(T t) : value_(make_shared<value_model<T>>(t)) {}

    /**
     * Retrieves the value of the computation as a type T.
     * The erased object type that models value must be convertible to
     * type T, otherwise nothing is returned (i.e., nullopt).
     */
    template <typename T>
    optional<T> as() const
    {
        auto t = dynamic_cast<value_model<T> const *>(value_.get());
        return t ? optional<T>(t->t) : nullopt;
    }

    ostream & out(ostream & out) const { return value_->out(out); }
    string get_type() const { return value_->get_type(); }
};

string type(value x) { return x.get_type(); }
ostream & operator<<(ostream & out, value const & r) { return r.out(out); }

