// Package value defines the runtime value types for the apertures language.
package value

import (
	"fmt"
	"strings"
)

// Value is the interface implemented by all aperture values.
// The unexported marker method seals the interface to this package.
type Value interface {
	String() string
	valueMarker()
}

// Nil represents the nil/null value.
type Nil struct{}

func (Nil) String() string  { return "nil" }
func (Nil) valueMarker()    {}

// Num represents a numeric value (float64).
type Num struct {
	Val float64
}

func (n Num) String() string {
	// Format nicely: integers without decimal point
	if n.Val == float64(int64(n.Val)) {
		return fmt.Sprintf("%d", int64(n.Val))
	}
	return fmt.Sprintf("%g", n.Val)
}
func (Num) valueMarker() {}

// Sym represents a symbol (identifier).
type Sym struct {
	Name string
}

func (s Sym) String() string { return s.Name }
func (Sym) valueMarker()     {}

// Str represents a string value.
type Str struct {
	Val string
}

func (s Str) String() string { return fmt.Sprintf("%q", s.Val) }
func (Str) valueMarker()     {}

// Bool represents a boolean value.
type Bool struct {
	Val bool
}

func (b Bool) String() string {
	if b.Val {
		return "true"
	}
	return "false"
}
func (Bool) valueMarker() {}

// Hole represents an unfilled aperture (hole) in an expression.
// Namespace is optional; if empty, it's a simple hole like ?x.
// If set, it's a namespaced hole like ?client.x.
type Hole struct {
	Namespace string
	Name      string
}

func (h Hole) String() string {
	if h.Namespace == "" {
		return "?" + h.Name
	}
	return "?" + h.Namespace + "." + h.Name
}
func (Hole) valueMarker() {}

// FullName returns the fully qualified hole name (ns.name or just name).
func (h Hole) FullName() string {
	if h.Namespace == "" {
		return h.Name
	}
	return h.Namespace + "." + h.Name
}

// List represents a list of values.
type List struct {
	Elements []Value
}

func (l List) String() string {
	if len(l.Elements) == 0 {
		return "()"
	}
	parts := make([]string, len(l.Elements))
	for i, e := range l.Elements {
		parts[i] = e.String()
	}
	return "(" + strings.Join(parts, " ") + ")"
}
func (List) valueMarker() {}

// Lambda represents a function closure.
type Lambda struct {
	Params []string
	Body   interface{} // *ast.Expr - forward reference avoided
	Env    interface{} // *env.Env - forward reference avoided
}

func (l Lambda) String() string {
	return fmt.Sprintf("(lambda (%s) ...)", strings.Join(l.Params, " "))
}
func (Lambda) valueMarker() {}

// Constructors for convenience

// NewNil creates a Nil value.
func NewNil() Nil { return Nil{} }

// NewNum creates a Num value.
func NewNum(v float64) Num { return Num{Val: v} }

// NewSym creates a Sym value.
func NewSym(name string) Sym { return Sym{Name: name} }

// NewStr creates a Str value.
func NewStr(v string) Str { return Str{Val: v} }

// NewBool creates a Bool value.
func NewBool(v bool) Bool { return Bool{Val: v} }

// NewHole creates a simple hole (no namespace).
func NewHole(name string) Hole { return Hole{Name: name} }

// NewNamespacedHole creates a namespaced hole.
func NewNamespacedHole(ns, name string) Hole { return Hole{Namespace: ns, Name: name} }

// NewList creates a List value.
func NewList(elements ...Value) List { return List{Elements: elements} }

// NewLambda creates a Lambda value.
func NewLambda(params []string, body, env interface{}) Lambda {
	return Lambda{Params: params, Body: body, Env: env}
}

// Type checking helpers

// IsNil returns true if v is Nil.
func IsNil(v Value) bool {
	_, ok := v.(Nil)
	return ok
}

// IsNum returns true if v is Num.
func IsNum(v Value) bool {
	_, ok := v.(Num)
	return ok
}

// IsSym returns true if v is Sym.
func IsSym(v Value) bool {
	_, ok := v.(Sym)
	return ok
}

// IsStr returns true if v is Str.
func IsStr(v Value) bool {
	_, ok := v.(Str)
	return ok
}

// IsBool returns true if v is Bool.
func IsBool(v Value) bool {
	_, ok := v.(Bool)
	return ok
}

// IsHole returns true if v is Hole.
func IsHole(v Value) bool {
	_, ok := v.(Hole)
	return ok
}

// IsList returns true if v is List.
func IsList(v Value) bool {
	_, ok := v.(List)
	return ok
}

// IsLambda returns true if v is Lambda.
func IsLambda(v Value) bool {
	_, ok := v.(Lambda)
	return ok
}

// Truthy returns whether a value is considered true in boolean context.
// false and nil are falsy; everything else is truthy.
func Truthy(v Value) bool {
	switch x := v.(type) {
	case Nil:
		return false
	case Bool:
		return x.Val
	default:
		return true
	}
}

// Equal returns whether two values are equal.
func Equal(a, b Value) bool {
	switch x := a.(type) {
	case Nil:
		_, ok := b.(Nil)
		return ok
	case Num:
		if y, ok := b.(Num); ok {
			return x.Val == y.Val
		}
	case Sym:
		if y, ok := b.(Sym); ok {
			return x.Name == y.Name
		}
	case Str:
		if y, ok := b.(Str); ok {
			return x.Val == y.Val
		}
	case Bool:
		if y, ok := b.(Bool); ok {
			return x.Val == y.Val
		}
	case Hole:
		if y, ok := b.(Hole); ok {
			return x.Namespace == y.Namespace && x.Name == y.Name
		}
	case List:
		if y, ok := b.(List); ok {
			if len(x.Elements) != len(y.Elements) {
				return false
			}
			for i := range x.Elements {
				if !Equal(x.Elements[i], y.Elements[i]) {
					return false
				}
			}
			return true
		}
	case Lambda:
		// Lambdas are only equal if they are the same object (identity)
		if y, ok := b.(Lambda); ok {
			return &x == &y
		}
	}
	return false
}
