package eval

import (
	"github.com/queelius/aperture/pkg/ast"
	"github.com/queelius/aperture/pkg/env"
	"github.com/queelius/aperture/pkg/value"
)

// PartialEval partially evaluates an expression, preserving holes.
// Returns the simplified expression with holes intact.
func PartialEval(expr ast.Expr, e *env.Env) ast.Expr {
	return partialEval(expr, e)
}

func partialEval(expr ast.Expr, e *env.Env) ast.Expr {
	switch x := expr.(type) {
	case *ast.AtomExpr:
		return partialAtom(x, e)
	case *ast.ListExpr:
		return partialList(x, e)
	case *ast.QuoteExpr:
		return x // Quoted expressions are not evaluated
	default:
		return expr
	}
}

func partialAtom(a *ast.AtomExpr, e *env.Env) ast.Expr {
	switch a.Kind {
	case ast.AtomSym:
		// Look up variable, return value if found, otherwise keep as symbol
		if val, ok := e.Lookup(a.StrVal); ok {
			return valueToExpr(val, a.Position)
		}
		return a
	case ast.AtomHole:
		// Holes are preserved
		return a
	default:
		// Constants evaluate to themselves
		return a
	}
}

func partialList(l *ast.ListExpr, e *env.Env) ast.Expr {
	if len(l.Elements) == 0 {
		return l
	}

	// Check for special forms
	if first, ok := l.Elements[0].(*ast.AtomExpr); ok && first.Kind == ast.AtomSym {
		switch first.StrVal {
		case "quote":
			return l
		case "if":
			return partialIf(l, e)
		case "cond":
			return partialCond(l, e)
		case "let":
			return partialLet(l, e)
		case "lambda":
			return partialLambda(l, e)
		case "define":
			return partialDefine(l, e)
		case "begin":
			return partialBegin(l, e)
		case "and":
			return partialAnd(l, e)
		case "or":
			return partialOr(l, e)
		case "+", "-", "*", "/":
			return partialArithmetic(l, e, first.StrVal)
		case "=", "<", ">", "<=", ">=":
			return partialComparison(l, e, first.StrVal)
		case "not":
			return partialNot(l, e)
		case "list", "head", "tail", "append", "length", "empty?", "nth":
			return partialListOp(l, e, first.StrVal)
		}
	}

	// General application: evaluate all parts
	elements := make([]ast.Expr, len(l.Elements))
	allGround := true
	for i, elem := range l.Elements {
		elements[i] = partialEval(elem, e)
		if ast.HasHoles(elements[i]) {
			allGround = false
		}
	}

	if allGround {
		// Try full evaluation
		result := safeEval(&ast.ListExpr{Position: l.Position, Elements: elements}, e)
		if result != nil {
			return valueToExpr(result, l.Position)
		}
	}

	return &ast.ListExpr{Position: l.Position, Elements: elements}
}

func partialIf(l *ast.ListExpr, e *env.Env) ast.Expr {
	if len(l.Elements) != 4 {
		return l
	}

	pred := partialEval(l.Elements[1], e)

	// If predicate has holes, block evaluation
	if ast.HasHoles(pred) {
		thenBranch := partialEval(l.Elements[2], e)
		elseBranch := partialEval(l.Elements[3], e)
		return &ast.ListExpr{
			Position: l.Position,
			Elements: []ast.Expr{l.Elements[0], pred, thenBranch, elseBranch},
		}
	}

	// Predicate is ground, evaluate it
	val := safeEval(pred, e)
	if val == nil {
		return l
	}

	if value.Truthy(val) {
		return partialEval(l.Elements[2], e)
	}
	return partialEval(l.Elements[3], e)
}

func partialCond(l *ast.ListExpr, e *env.Env) ast.Expr {
	var newClauses []ast.Expr
	newClauses = append(newClauses, l.Elements[0]) // cond keyword

	for i := 1; i < len(l.Elements); i++ {
		clause, ok := l.Elements[i].(*ast.ListExpr)
		if !ok || len(clause.Elements) < 2 {
			continue
		}

		// Check for else clause
		if first, ok := clause.Elements[0].(*ast.AtomExpr); ok && first.Kind == ast.AtomSym && first.StrVal == "else" {
			body := partialEval(clause.Elements[1], e)
			newClauses = append(newClauses, &ast.ListExpr{
				Position: clause.Position,
				Elements: []ast.Expr{clause.Elements[0], body},
			})
			break
		}

		pred := partialEval(clause.Elements[0], e)

		if ast.HasHoles(pred) {
			// Can't determine this clause, keep it and continue
			body := partialEval(clause.Elements[1], e)
			newClauses = append(newClauses, &ast.ListExpr{
				Position: clause.Position,
				Elements: []ast.Expr{pred, body},
			})
			continue
		}

		// Predicate is ground
		val := safeEval(pred, e)
		if val != nil && value.Truthy(val) {
			// This clause matches, return its body
			return partialEval(clause.Elements[1], e)
		}
		// Predicate is false, skip this clause
	}

	if len(newClauses) == 1 {
		// All clauses eliminated, return nil
		return ast.Nil(l.Position)
	}

	return &ast.ListExpr{Position: l.Position, Elements: newClauses}
}

func partialLet(l *ast.ListExpr, e *env.Env) ast.Expr {
	if len(l.Elements) < 3 {
		return l
	}

	bindings, ok := l.Elements[1].(*ast.ListExpr)
	if !ok {
		return l
	}

	// Create new environment and process bindings
	newEnv := env.NewWithParent(e)
	var newBindings []ast.Expr
	allGround := true

	for _, binding := range bindings.Elements {
		bl, ok := binding.(*ast.ListExpr)
		if !ok || len(bl.Elements) != 2 {
			continue
		}
		name, ok := bl.Elements[0].(*ast.AtomExpr)
		if !ok || name.Kind != ast.AtomSym {
			continue
		}

		valExpr := partialEval(bl.Elements[1], newEnv)

		if !ast.HasHoles(valExpr) {
			// Ground value, add to environment
			val := safeEval(valExpr, newEnv)
			if val != nil {
				newEnv.Define(name.StrVal, val)
			}
		} else {
			allGround = false
		}

		newBindings = append(newBindings, &ast.ListExpr{
			Position: bl.Position,
			Elements: []ast.Expr{name, valExpr},
		})
	}

	// Evaluate body
	var bodyExprs []ast.Expr
	for i := 2; i < len(l.Elements); i++ {
		bodyExprs = append(bodyExprs, partialEval(l.Elements[i], newEnv))
	}

	// If all bindings are ground and body is ground, fully evaluate
	if allGround && len(bodyExprs) > 0 {
		lastBody := bodyExprs[len(bodyExprs)-1]
		if !ast.HasHoles(lastBody) {
			val := safeEval(lastBody, newEnv)
			if val != nil {
				return valueToExpr(val, l.Position)
			}
		}
	}

	// Reconstruct let with simplified parts
	newBindingList := &ast.ListExpr{Position: bindings.Position, Elements: newBindings}
	result := []ast.Expr{l.Elements[0], newBindingList}
	result = append(result, bodyExprs...)
	return &ast.ListExpr{Position: l.Position, Elements: result}
}

func partialLambda(l *ast.ListExpr, e *env.Env) ast.Expr {
	// Lambdas are returned as-is during partial evaluation
	// They capture their environment but body is not evaluated
	return l
}

func partialDefine(l *ast.ListExpr, e *env.Env) ast.Expr {
	if len(l.Elements) < 3 {
		return l
	}

	// For function definition shorthand, keep as-is
	if _, ok := l.Elements[1].(*ast.ListExpr); ok {
		return l
	}

	// Simple definition: partially evaluate the value
	name, ok := l.Elements[1].(*ast.AtomExpr)
	if !ok || name.Kind != ast.AtomSym {
		return l
	}

	valExpr := partialEval(l.Elements[2], e)

	// If value is ground, add to environment
	if !ast.HasHoles(valExpr) {
		val := safeEval(valExpr, e)
		if val != nil {
			e.Define(name.StrVal, val)
			return valueToExpr(val, l.Position)
		}
	}

	return &ast.ListExpr{
		Position: l.Position,
		Elements: []ast.Expr{l.Elements[0], name, valExpr},
	}
}

func partialBegin(l *ast.ListExpr, e *env.Env) ast.Expr {
	if len(l.Elements) == 1 {
		return ast.Nil(l.Position)
	}

	var exprs []ast.Expr
	exprs = append(exprs, l.Elements[0])

	for i := 1; i < len(l.Elements); i++ {
		exprs = append(exprs, partialEval(l.Elements[i], e))
	}

	// If last expression is ground, return it
	last := exprs[len(exprs)-1]
	if !ast.HasHoles(last) {
		return last
	}

	return &ast.ListExpr{Position: l.Position, Elements: exprs}
}

func partialAnd(l *ast.ListExpr, e *env.Env) ast.Expr {
	if len(l.Elements) == 1 {
		return ast.Bool(l.Position, true)
	}

	var exprs []ast.Expr
	exprs = append(exprs, l.Elements[0])

	for i := 1; i < len(l.Elements); i++ {
		arg := partialEval(l.Elements[i], e)

		if !ast.HasHoles(arg) {
			val := safeEval(arg, e)
			if val != nil && !value.Truthy(val) {
				// Short-circuit: found falsy value
				return valueToExpr(val, l.Position)
			}
			// Truthy ground value, continue
			if i == len(l.Elements)-1 {
				// Last element and truthy
				return arg
			}
		} else {
			exprs = append(exprs, arg)
		}
	}

	if len(exprs) == 1 {
		return ast.Bool(l.Position, true)
	}

	return &ast.ListExpr{Position: l.Position, Elements: exprs}
}

func partialOr(l *ast.ListExpr, e *env.Env) ast.Expr {
	if len(l.Elements) == 1 {
		return ast.Bool(l.Position, false)
	}

	var exprs []ast.Expr
	exprs = append(exprs, l.Elements[0])

	for i := 1; i < len(l.Elements); i++ {
		arg := partialEval(l.Elements[i], e)

		if !ast.HasHoles(arg) {
			val := safeEval(arg, e)
			if val != nil && value.Truthy(val) {
				// Short-circuit: found truthy value
				return valueToExpr(val, l.Position)
			}
			// Falsy ground value, continue
		} else {
			exprs = append(exprs, arg)
		}
	}

	if len(exprs) == 1 {
		return ast.Bool(l.Position, false)
	}

	return &ast.ListExpr{Position: l.Position, Elements: exprs}
}

func partialArithmetic(l *ast.ListExpr, e *env.Env, op string) ast.Expr {
	if len(l.Elements) == 1 {
		// No arguments
		switch op {
		case "+":
			return ast.Num(l.Position, 0)
		case "*":
			return ast.Num(l.Position, 1)
		default:
			return l
		}
	}

	// Evaluate arguments
	args := make([]ast.Expr, len(l.Elements)-1)
	hasHoles := false
	for i := 1; i < len(l.Elements); i++ {
		args[i-1] = partialEval(l.Elements[i], e)
		if ast.HasHoles(args[i-1]) {
			hasHoles = true
		}
	}

	if !hasHoles {
		// All ground, fully evaluate
		result := safeEval(&ast.ListExpr{
			Position: l.Position,
			Elements: append([]ast.Expr{l.Elements[0]}, args...),
		}, e)
		if result != nil {
			return valueToExpr(result, l.Position)
		}
	}

	// Apply algebraic simplifications
	return Simplify(&ast.ListExpr{
		Position: l.Position,
		Elements: append([]ast.Expr{l.Elements[0]}, args...),
	})
}

func partialComparison(l *ast.ListExpr, e *env.Env, op string) ast.Expr {
	if len(l.Elements) != 3 {
		return l
	}

	left := partialEval(l.Elements[1], e)
	right := partialEval(l.Elements[2], e)

	if !ast.HasHoles(left) && !ast.HasHoles(right) {
		result := safeEval(&ast.ListExpr{
			Position: l.Position,
			Elements: []ast.Expr{l.Elements[0], left, right},
		}, e)
		if result != nil {
			return valueToExpr(result, l.Position)
		}
	}

	return &ast.ListExpr{
		Position: l.Position,
		Elements: []ast.Expr{l.Elements[0], left, right},
	}
}

func partialNot(l *ast.ListExpr, e *env.Env) ast.Expr {
	if len(l.Elements) != 2 {
		return l
	}

	arg := partialEval(l.Elements[1], e)

	if !ast.HasHoles(arg) {
		result := safeEval(&ast.ListExpr{
			Position: l.Position,
			Elements: []ast.Expr{l.Elements[0], arg},
		}, e)
		if result != nil {
			return valueToExpr(result, l.Position)
		}
	}

	return &ast.ListExpr{
		Position: l.Position,
		Elements: []ast.Expr{l.Elements[0], arg},
	}
}

func partialListOp(l *ast.ListExpr, e *env.Env, op string) ast.Expr {
	// Evaluate all arguments
	args := make([]ast.Expr, len(l.Elements)-1)
	hasHoles := false
	for i := 1; i < len(l.Elements); i++ {
		args[i-1] = partialEval(l.Elements[i], e)
		if ast.HasHoles(args[i-1]) {
			hasHoles = true
		}
	}

	if !hasHoles {
		result := safeEval(&ast.ListExpr{
			Position: l.Position,
			Elements: append([]ast.Expr{l.Elements[0]}, args...),
		}, e)
		if result != nil {
			return valueToExpr(result, l.Position)
		}
	}

	return &ast.ListExpr{
		Position: l.Position,
		Elements: append([]ast.Expr{l.Elements[0]}, args...),
	}
}

// safeEval evaluates an expression, returning nil on panic.
func safeEval(expr ast.Expr, e *env.Env) (result value.Value) {
	defer func() {
		if r := recover(); r != nil {
			result = nil
		}
	}()
	return Eval(expr, e)
}

// valueToExpr converts a value back to an AST expression.
func valueToExpr(v value.Value, pos ast.Pos) ast.Expr {
	switch x := v.(type) {
	case value.Nil:
		return ast.Nil(pos)
	case value.Num:
		return ast.Num(pos, x.Val)
	case value.Str:
		return ast.Str(pos, x.Val)
	case value.Bool:
		return ast.Bool(pos, x.Val)
	case value.Sym:
		return ast.Sym(pos, x.Name)
	case value.Hole:
		if x.Namespace == "" {
			return ast.Hole(pos, x.Name)
		}
		return ast.NamespacedHole(pos, x.Namespace, x.Name)
	case value.List:
		elements := make([]ast.Expr, len(x.Elements))
		for i, e := range x.Elements {
			elements[i] = valueToExpr(e, pos)
		}
		return &ast.ListExpr{Position: pos, Elements: elements}
	case value.Lambda:
		// Can't convert lambdas back easily, return as-is for now
		params := make([]ast.Expr, len(x.Params))
		for i, p := range x.Params {
			params[i] = ast.Sym(pos, p)
		}
		return &ast.ListExpr{
			Position: pos,
			Elements: []ast.Expr{
				ast.Sym(pos, "lambda"),
				&ast.ListExpr{Position: pos, Elements: params},
				x.Body.(ast.Expr),
			},
		}
	default:
		return ast.Nil(pos)
	}
}

// Fill replaces holes in an expression with the given bindings.
func Fill(expr ast.Expr, bindings map[string]value.Value) ast.Expr {
	return fill(expr, bindings)
}

func fill(expr ast.Expr, bindings map[string]value.Value) ast.Expr {
	switch x := expr.(type) {
	case *ast.AtomExpr:
		if x.Kind == ast.AtomHole {
			name := ast.HoleName(x)
			if val, ok := bindings[name]; ok {
				return valueToExpr(val, x.Position)
			}
		}
		return x
	case *ast.ListExpr:
		elements := make([]ast.Expr, len(x.Elements))
		for i, elem := range x.Elements {
			elements[i] = fill(elem, bindings)
		}
		return &ast.ListExpr{Position: x.Position, Elements: elements}
	case *ast.QuoteExpr:
		return &ast.QuoteExpr{Position: x.Position, Inner: fill(x.Inner, bindings)}
	default:
		return expr
	}
}

// Holes returns all hole names in an expression.
func Holes(expr ast.Expr) []string {
	return ast.CollectHoles(expr)
}

// IsGround returns true if the expression has no holes.
func IsGround(expr ast.Expr) bool {
	return !ast.HasHoles(expr)
}
