// Package ast defines the abstract syntax tree types for apertures.
package ast

import (
	"fmt"
	"strings"
)

// Pos represents a position in source code.
type Pos struct {
	Line   int
	Column int
}

func (p Pos) String() string {
	return fmt.Sprintf("%d:%d", p.Line, p.Column)
}

// Expr is the interface for all AST expression nodes.
type Expr interface {
	String() string
	Pos() Pos
	exprMarker()
}

// AtomExpr represents atomic values: numbers, symbols, strings, booleans, nil, holes.
type AtomExpr struct {
	Position Pos
	Kind     AtomKind
	// Value holds the actual data:
	// - NumVal for numbers
	// - StrVal for strings and symbols
	// - BoolVal for booleans
	// - For holes: StrVal is the name, Namespace is optional namespace
	NumVal    float64
	StrVal    string
	BoolVal   bool
	Namespace string // for namespaced holes
}

// AtomKind identifies the type of atom.
type AtomKind int

const (
	AtomNil AtomKind = iota
	AtomNum
	AtomSym
	AtomStr
	AtomBool
	AtomHole
)

func (a *AtomExpr) String() string {
	switch a.Kind {
	case AtomNil:
		return "nil"
	case AtomNum:
		if a.NumVal == float64(int64(a.NumVal)) {
			return fmt.Sprintf("%d", int64(a.NumVal))
		}
		return fmt.Sprintf("%g", a.NumVal)
	case AtomSym:
		return a.StrVal
	case AtomStr:
		return fmt.Sprintf("%q", a.StrVal)
	case AtomBool:
		if a.BoolVal {
			return "true"
		}
		return "false"
	case AtomHole:
		if a.Namespace == "" {
			return "?" + a.StrVal
		}
		return "?" + a.Namespace + "." + a.StrVal
	default:
		return "<unknown atom>"
	}
}

func (a *AtomExpr) Pos() Pos  { return a.Position }
func (*AtomExpr) exprMarker() {}

// ListExpr represents a list/application: (op arg1 arg2 ...)
type ListExpr struct {
	Position Pos
	Elements []Expr
}

func (l *ListExpr) String() string {
	if len(l.Elements) == 0 {
		return "()"
	}
	parts := make([]string, len(l.Elements))
	for i, e := range l.Elements {
		parts[i] = e.String()
	}
	return "(" + strings.Join(parts, " ") + ")"
}

func (l *ListExpr) Pos() Pos  { return l.Position }
func (*ListExpr) exprMarker() {}

// QuoteExpr represents quoted expressions: 'expr or (quote expr)
type QuoteExpr struct {
	Position Pos
	Inner    Expr
}

func (q *QuoteExpr) String() string {
	return "'" + q.Inner.String()
}

func (q *QuoteExpr) Pos() Pos   { return q.Position }
func (*QuoteExpr) exprMarker() {}

// Helper constructors

// Nil creates a nil atom at the given position.
func Nil(pos Pos) *AtomExpr {
	return &AtomExpr{Position: pos, Kind: AtomNil}
}

// Num creates a number atom.
func Num(pos Pos, val float64) *AtomExpr {
	return &AtomExpr{Position: pos, Kind: AtomNum, NumVal: val}
}

// Sym creates a symbol atom.
func Sym(pos Pos, name string) *AtomExpr {
	return &AtomExpr{Position: pos, Kind: AtomSym, StrVal: name}
}

// Str creates a string atom.
func Str(pos Pos, val string) *AtomExpr {
	return &AtomExpr{Position: pos, Kind: AtomStr, StrVal: val}
}

// Bool creates a boolean atom.
func Bool(pos Pos, val bool) *AtomExpr {
	return &AtomExpr{Position: pos, Kind: AtomBool, BoolVal: val}
}

// Hole creates a simple hole atom.
func Hole(pos Pos, name string) *AtomExpr {
	return &AtomExpr{Position: pos, Kind: AtomHole, StrVal: name}
}

// NamespacedHole creates a namespaced hole atom.
func NamespacedHole(pos Pos, ns, name string) *AtomExpr {
	return &AtomExpr{Position: pos, Kind: AtomHole, Namespace: ns, StrVal: name}
}

// List creates a list expression.
func List(pos Pos, elements ...Expr) *ListExpr {
	return &ListExpr{Position: pos, Elements: elements}
}

// Quote creates a quoted expression.
func Quote(pos Pos, inner Expr) *QuoteExpr {
	return &QuoteExpr{Position: pos, Inner: inner}
}

// IsAtom returns true if the expression is an atom.
func IsAtom(e Expr) bool {
	_, ok := e.(*AtomExpr)
	return ok
}

// IsList returns true if the expression is a list.
func IsList(e Expr) bool {
	_, ok := e.(*ListExpr)
	return ok
}

// IsQuote returns true if the expression is a quote.
func IsQuote(e Expr) bool {
	_, ok := e.(*QuoteExpr)
	return ok
}

// IsHoleExpr returns true if the expression is a hole.
func IsHoleExpr(e Expr) bool {
	if a, ok := e.(*AtomExpr); ok {
		return a.Kind == AtomHole
	}
	return false
}

// HoleName returns the full name of a hole expression (ns.name or name).
// Returns empty string if not a hole.
func HoleName(e Expr) string {
	if a, ok := e.(*AtomExpr); ok && a.Kind == AtomHole {
		if a.Namespace == "" {
			return a.StrVal
		}
		return a.Namespace + "." + a.StrVal
	}
	return ""
}

// CollectHoles returns all hole names in an expression.
func CollectHoles(e Expr) []string {
	var holes []string
	seen := make(map[string]bool)

	var walk func(Expr)
	walk = func(e Expr) {
		switch x := e.(type) {
		case *AtomExpr:
			if x.Kind == AtomHole {
				name := HoleName(x)
				if !seen[name] {
					seen[name] = true
					holes = append(holes, name)
				}
			}
		case *ListExpr:
			for _, elem := range x.Elements {
				walk(elem)
			}
		case *QuoteExpr:
			walk(x.Inner)
		}
	}
	walk(e)
	return holes
}

// HasHoles returns true if the expression contains any holes.
func HasHoles(e Expr) bool {
	switch x := e.(type) {
	case *AtomExpr:
		return x.Kind == AtomHole
	case *ListExpr:
		for _, elem := range x.Elements {
			if HasHoles(elem) {
				return true
			}
		}
		return false
	case *QuoteExpr:
		return HasHoles(x.Inner)
	}
	return false
}

// Clone creates a deep copy of an expression.
func Clone(e Expr) Expr {
	switch x := e.(type) {
	case *AtomExpr:
		copy := *x
		return &copy
	case *ListExpr:
		elements := make([]Expr, len(x.Elements))
		for i, elem := range x.Elements {
			elements[i] = Clone(elem)
		}
		return &ListExpr{Position: x.Position, Elements: elements}
	case *QuoteExpr:
		return &QuoteExpr{Position: x.Position, Inner: Clone(x.Inner)}
	}
	return e
}
