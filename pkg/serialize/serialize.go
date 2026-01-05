// Package serialize provides serialization for aperture expressions.
package serialize

import (
	"github.com/queelius/aperture/pkg/ast"
	"github.com/queelius/aperture/pkg/parser"
)

// Serialize converts an expression to its string representation.
func Serialize(expr ast.Expr) string {
	return expr.String()
}

// Deserialize parses a string into an expression.
func Deserialize(s string) (ast.Expr, error) {
	return parser.ParseOne(s)
}

// DeserializeAll parses a string into multiple expressions.
func DeserializeAll(s string) ([]ast.Expr, error) {
	return parser.Parse(s)
}
