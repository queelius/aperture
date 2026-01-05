package eval

import (
	"github.com/queelius/aperture/pkg/ast"
)

// Simplify applies algebraic simplifications to an expression.
// This includes identity rules, annihilation, and constant folding.
func Simplify(expr ast.Expr) ast.Expr {
	return simplify(expr)
}

func simplify(expr ast.Expr) ast.Expr {
	l, ok := expr.(*ast.ListExpr)
	if !ok || len(l.Elements) == 0 {
		return expr
	}

	first, ok := l.Elements[0].(*ast.AtomExpr)
	if !ok || first.Kind != ast.AtomSym {
		return expr
	}

	switch first.StrVal {
	case "+":
		return simplifyAdd(l)
	case "-":
		return simplifySub(l)
	case "*":
		return simplifyMul(l)
	case "/":
		return simplifyDiv(l)
	case "if":
		return simplifyIf(l)
	default:
		return expr
	}
}

func simplifyAdd(l *ast.ListExpr) ast.Expr {
	if len(l.Elements) == 1 {
		return ast.Num(l.Position, 0)
	}

	// Collect constants and non-constants
	var constSum float64
	var nonConsts []ast.Expr

	for i := 1; i < len(l.Elements); i++ {
		arg := l.Elements[i]
		if a, ok := arg.(*ast.AtomExpr); ok && a.Kind == ast.AtomNum {
			constSum += a.NumVal
		} else {
			nonConsts = append(nonConsts, arg)
		}
	}

	// All constants
	if len(nonConsts) == 0 {
		return ast.Num(l.Position, constSum)
	}

	// Only non-constants, no constant contribution
	if constSum == 0 {
		if len(nonConsts) == 1 {
			return nonConsts[0]
		}
		return &ast.ListExpr{
			Position: l.Position,
			Elements: append([]ast.Expr{l.Elements[0]}, nonConsts...),
		}
	}

	// Mix of constants and non-constants
	result := append([]ast.Expr{l.Elements[0], ast.Num(l.Position, constSum)}, nonConsts...)
	return &ast.ListExpr{Position: l.Position, Elements: result}
}

func simplifySub(l *ast.ListExpr) ast.Expr {
	if len(l.Elements) == 1 {
		return l // Error case, leave as-is
	}

	// Unary minus
	if len(l.Elements) == 2 {
		if a, ok := l.Elements[1].(*ast.AtomExpr); ok && a.Kind == ast.AtomNum {
			return ast.Num(l.Position, -a.NumVal)
		}
		return l
	}

	// (- x 0) → x
	if len(l.Elements) == 3 {
		if a, ok := l.Elements[2].(*ast.AtomExpr); ok && a.Kind == ast.AtomNum && a.NumVal == 0 {
			return l.Elements[1]
		}
	}

	// Try to fold constants
	first, ok1 := l.Elements[1].(*ast.AtomExpr)
	if ok1 && first.Kind == ast.AtomNum {
		allConst := true
		result := first.NumVal
		for i := 2; i < len(l.Elements); i++ {
			if a, ok := l.Elements[i].(*ast.AtomExpr); ok && a.Kind == ast.AtomNum {
				result -= a.NumVal
			} else {
				allConst = false
				break
			}
		}
		if allConst {
			return ast.Num(l.Position, result)
		}
	}

	return l
}

func simplifyMul(l *ast.ListExpr) ast.Expr {
	if len(l.Elements) == 1 {
		return ast.Num(l.Position, 1)
	}

	// Check for zero - annihilation
	for i := 1; i < len(l.Elements); i++ {
		if a, ok := l.Elements[i].(*ast.AtomExpr); ok && a.Kind == ast.AtomNum && a.NumVal == 0 {
			return ast.Num(l.Position, 0)
		}
	}

	// Collect constants and non-constants
	constProduct := 1.0
	var nonConsts []ast.Expr

	for i := 1; i < len(l.Elements); i++ {
		arg := l.Elements[i]
		if a, ok := arg.(*ast.AtomExpr); ok && a.Kind == ast.AtomNum {
			constProduct *= a.NumVal
		} else {
			nonConsts = append(nonConsts, arg)
		}
	}

	// All constants
	if len(nonConsts) == 0 {
		return ast.Num(l.Position, constProduct)
	}

	// Product is zero (shouldn't happen, caught above)
	if constProduct == 0 {
		return ast.Num(l.Position, 0)
	}

	// Only non-constants, constant is 1 (identity)
	if constProduct == 1 {
		if len(nonConsts) == 1 {
			return nonConsts[0]
		}
		return &ast.ListExpr{
			Position: l.Position,
			Elements: append([]ast.Expr{l.Elements[0]}, nonConsts...),
		}
	}

	// Mix of constants and non-constants
	result := append([]ast.Expr{l.Elements[0], ast.Num(l.Position, constProduct)}, nonConsts...)
	return &ast.ListExpr{Position: l.Position, Elements: result}
}

func simplifyDiv(l *ast.ListExpr) ast.Expr {
	if len(l.Elements) == 1 {
		return l // Error case
	}

	// (/ 0 x) → 0 (if x is not zero)
	if len(l.Elements) >= 3 {
		if a, ok := l.Elements[1].(*ast.AtomExpr); ok && a.Kind == ast.AtomNum && a.NumVal == 0 {
			return ast.Num(l.Position, 0)
		}
	}

	// (/ x 1) → x
	if len(l.Elements) == 3 {
		if a, ok := l.Elements[2].(*ast.AtomExpr); ok && a.Kind == ast.AtomNum && a.NumVal == 1 {
			return l.Elements[1]
		}
	}

	// Try to fold all constants
	if len(l.Elements) >= 2 {
		first, ok1 := l.Elements[1].(*ast.AtomExpr)
		if ok1 && first.Kind == ast.AtomNum {
			allConst := true
			result := first.NumVal
			for i := 2; i < len(l.Elements); i++ {
				if a, ok := l.Elements[i].(*ast.AtomExpr); ok && a.Kind == ast.AtomNum {
					if a.NumVal == 0 {
						return l // Division by zero, leave as-is
					}
					result /= a.NumVal
				} else {
					allConst = false
					break
				}
			}
			if allConst && len(l.Elements) > 2 {
				return ast.Num(l.Position, result)
			}
		}
	}

	return l
}

func simplifyIf(l *ast.ListExpr) ast.Expr {
	if len(l.Elements) != 4 {
		return l
	}

	pred := l.Elements[1]

	// (if true then else) → then
	if a, ok := pred.(*ast.AtomExpr); ok {
		if a.Kind == ast.AtomBool {
			if a.BoolVal {
				return l.Elements[2]
			}
			return l.Elements[3]
		}
		// (if nil then else) → else
		if a.Kind == ast.AtomNil {
			return l.Elements[3]
		}
		// (if <non-nil-atom> then else) → then (for numbers, strings, etc.)
		if a.Kind == ast.AtomNum || a.Kind == ast.AtomStr || a.Kind == ast.AtomSym {
			return l.Elements[2]
		}
	}

	return l
}
