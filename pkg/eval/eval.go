// Package eval provides evaluation for the apertures language.
package eval

import (
	"fmt"

	"github.com/queelius/aperture/pkg/ast"
	"github.com/queelius/aperture/pkg/env"
	"github.com/queelius/aperture/pkg/value"
)

// Eval evaluates an expression in the given environment.
// Panics on errors (type mismatches, undefined variables, holes, etc.).
func Eval(expr ast.Expr, e *env.Env) value.Value {
	switch x := expr.(type) {
	case *ast.AtomExpr:
		return evalAtom(x, e)
	case *ast.ListExpr:
		return evalList(x, e)
	case *ast.QuoteExpr:
		return exprToValue(x.Inner)
	default:
		panic(fmt.Sprintf("unknown expression type: %T", expr))
	}
}

func evalAtom(a *ast.AtomExpr, e *env.Env) value.Value {
	switch a.Kind {
	case ast.AtomNil:
		return value.NewNil()
	case ast.AtomNum:
		return value.NewNum(a.NumVal)
	case ast.AtomStr:
		return value.NewStr(a.StrVal)
	case ast.AtomBool:
		return value.NewBool(a.BoolVal)
	case ast.AtomSym:
		return e.MustLookup(a.StrVal)
	case ast.AtomHole:
		panic(fmt.Sprintf("cannot evaluate unfilled hole: %s at %s", a.String(), a.Position))
	default:
		panic(fmt.Sprintf("unknown atom kind: %d", a.Kind))
	}
}

func evalList(l *ast.ListExpr, e *env.Env) value.Value {
	if len(l.Elements) == 0 {
		return value.NewList()
	}

	// Check for special forms and builtins
	if first, ok := l.Elements[0].(*ast.AtomExpr); ok && first.Kind == ast.AtomSym {
		switch first.StrVal {
		// Special forms
		case "quote":
			return evalQuote(l, e)
		case "if":
			return evalIf(l, e)
		case "cond":
			return evalCond(l, e)
		case "let":
			return evalLet(l, e)
		case "lambda":
			return evalLambda(l, e)
		case "define":
			return evalDefine(l, e)
		case "begin":
			return evalBegin(l, e)
		case "and":
			return evalAnd(l, e)
		case "or":
			return evalOr(l, e)
		// Builtins - evaluate args and apply
		case "+", "-", "*", "/", "=", "<", ">", "<=", ">=", "not",
			"list", "head", "tail", "append", "length", "empty?", "nth",
			"holes", "ground?":
			return evalBuiltin(l, e, first.StrVal)
		}
	}

	// Function application
	return evalApplication(l, e)
}

func evalQuote(l *ast.ListExpr, e *env.Env) value.Value {
	if len(l.Elements) != 2 {
		panic("quote requires exactly one argument")
	}
	return exprToValue(l.Elements[1])
}

func evalIf(l *ast.ListExpr, e *env.Env) value.Value {
	if len(l.Elements) != 4 {
		panic("if requires exactly 3 arguments: (if pred then else)")
	}
	pred := Eval(l.Elements[1], e)
	if value.Truthy(pred) {
		return Eval(l.Elements[2], e)
	}
	return Eval(l.Elements[3], e)
}

func evalCond(l *ast.ListExpr, e *env.Env) value.Value {
	for i := 1; i < len(l.Elements); i++ {
		clause, ok := l.Elements[i].(*ast.ListExpr)
		if !ok || len(clause.Elements) < 2 {
			panic("cond clause must be a list with at least 2 elements")
		}

		// Check for else clause
		if first, ok := clause.Elements[0].(*ast.AtomExpr); ok && first.Kind == ast.AtomSym && first.StrVal == "else" {
			return Eval(clause.Elements[1], e)
		}

		pred := Eval(clause.Elements[0], e)
		if value.Truthy(pred) {
			return Eval(clause.Elements[1], e)
		}
	}
	return value.NewNil()
}

func evalLet(l *ast.ListExpr, e *env.Env) value.Value {
	if len(l.Elements) < 3 {
		panic("let requires bindings and body: (let ((x e) ...) body)")
	}
	bindings, ok := l.Elements[1].(*ast.ListExpr)
	if !ok {
		panic("let bindings must be a list")
	}

	// Create new environment
	newEnv := env.NewWithParent(e)

	// Process bindings sequentially
	for _, binding := range bindings.Elements {
		bl, ok := binding.(*ast.ListExpr)
		if !ok || len(bl.Elements) != 2 {
			panic("let binding must be (name expr)")
		}
		name, ok := bl.Elements[0].(*ast.AtomExpr)
		if !ok || name.Kind != ast.AtomSym {
			panic("let binding name must be a symbol")
		}
		val := Eval(bl.Elements[1], newEnv)
		newEnv.Define(name.StrVal, val)
	}

	// Evaluate body expressions
	var result value.Value = value.NewNil()
	for i := 2; i < len(l.Elements); i++ {
		result = Eval(l.Elements[i], newEnv)
	}
	return result
}

func evalLambda(l *ast.ListExpr, e *env.Env) value.Value {
	if len(l.Elements) < 3 {
		panic("lambda requires parameters and body: (lambda (params) body)")
	}
	params, ok := l.Elements[1].(*ast.ListExpr)
	if !ok {
		panic("lambda parameters must be a list")
	}

	// Extract parameter names
	paramNames := make([]string, len(params.Elements))
	for i, p := range params.Elements {
		pa, ok := p.(*ast.AtomExpr)
		if !ok || pa.Kind != ast.AtomSym {
			panic("lambda parameter must be a symbol")
		}
		paramNames[i] = pa.StrVal
	}

	// Combine body expressions into a begin if multiple
	var body ast.Expr
	if len(l.Elements) == 3 {
		body = l.Elements[2]
	} else {
		body = &ast.ListExpr{
			Position: l.Position,
			Elements: append([]ast.Expr{ast.Sym(l.Position, "begin")}, l.Elements[2:]...),
		}
	}

	return value.NewLambda(paramNames, body, e)
}

func evalDefine(l *ast.ListExpr, e *env.Env) value.Value {
	if len(l.Elements) < 3 {
		panic("define requires name and value: (define name expr)")
	}

	// Check for function definition shorthand: (define (name args...) body)
	if fn, ok := l.Elements[1].(*ast.ListExpr); ok && len(fn.Elements) > 0 {
		name, ok := fn.Elements[0].(*ast.AtomExpr)
		if !ok || name.Kind != ast.AtomSym {
			panic("define function name must be a symbol")
		}

		// Build lambda expression
		params := &ast.ListExpr{Position: fn.Position, Elements: fn.Elements[1:]}
		lambdaExpr := &ast.ListExpr{
			Position: l.Position,
			Elements: append([]ast.Expr{ast.Sym(l.Position, "lambda"), params}, l.Elements[2:]...),
		}
		val := Eval(lambdaExpr, e)
		e.Define(name.StrVal, val)
		return val
	}

	// Simple definition: (define name expr)
	name, ok := l.Elements[1].(*ast.AtomExpr)
	if !ok || name.Kind != ast.AtomSym {
		panic("define name must be a symbol")
	}
	val := Eval(l.Elements[2], e)
	e.Define(name.StrVal, val)
	return val
}

func evalBegin(l *ast.ListExpr, e *env.Env) value.Value {
	var result value.Value = value.NewNil()
	for i := 1; i < len(l.Elements); i++ {
		result = Eval(l.Elements[i], e)
	}
	return result
}

func evalAnd(l *ast.ListExpr, e *env.Env) value.Value {
	var result value.Value = value.NewBool(true)
	for i := 1; i < len(l.Elements); i++ {
		result = Eval(l.Elements[i], e)
		if !value.Truthy(result) {
			return result
		}
	}
	return result
}

func evalOr(l *ast.ListExpr, e *env.Env) value.Value {
	var result value.Value = value.NewBool(false)
	for i := 1; i < len(l.Elements); i++ {
		result = Eval(l.Elements[i], e)
		if value.Truthy(result) {
			return result
		}
	}
	return result
}

func evalBuiltin(l *ast.ListExpr, e *env.Env, name string) value.Value {
	// Evaluate arguments
	args := make([]value.Value, len(l.Elements)-1)
	for i := 1; i < len(l.Elements); i++ {
		args[i-1] = Eval(l.Elements[i], e)
	}
	return applyBuiltin(name, args)
}

func evalApplication(l *ast.ListExpr, e *env.Env) value.Value {
	// Evaluate operator
	op := Eval(l.Elements[0], e)

	// Evaluate arguments
	args := make([]value.Value, len(l.Elements)-1)
	for i := 1; i < len(l.Elements); i++ {
		args[i-1] = Eval(l.Elements[i], e)
	}

	// Apply based on operator type
	switch fn := op.(type) {
	case value.Lambda:
		return applyLambda(fn, args)
	case value.Sym:
		return applyBuiltin(fn.Name, args)
	default:
		// Check if operator is a symbol for builtins
		if first, ok := l.Elements[0].(*ast.AtomExpr); ok && first.Kind == ast.AtomSym {
			return applyBuiltin(first.StrVal, args)
		}
		panic(fmt.Sprintf("cannot apply non-function: %v", op))
	}
}

func applyLambda(fn value.Lambda, args []value.Value) value.Value {
	if len(args) != len(fn.Params) {
		panic(fmt.Sprintf("lambda expects %d arguments, got %d", len(fn.Params), len(args)))
	}
	capturedEnv := fn.Env.(*env.Env)
	newEnv := capturedEnv.Extend(fn.Params, args)
	return Eval(fn.Body.(ast.Expr), newEnv)
}

func applyBuiltin(name string, args []value.Value) value.Value {
	switch name {
	// Arithmetic
	case "+":
		return builtinAdd(args)
	case "-":
		return builtinSub(args)
	case "*":
		return builtinMul(args)
	case "/":
		return builtinDiv(args)

	// Comparison
	case "=":
		return builtinEq(args)
	case "<":
		return builtinLt(args)
	case ">":
		return builtinGt(args)
	case "<=":
		return builtinLe(args)
	case ">=":
		return builtinGe(args)

	// Logic
	case "not":
		return builtinNot(args)

	// List operations
	case "list":
		return builtinList(args)
	case "head":
		return builtinHead(args)
	case "tail":
		return builtinTail(args)
	case "append":
		return builtinAppend(args)
	case "length":
		return builtinLength(args)
	case "empty?":
		return builtinEmpty(args)
	case "nth":
		return builtinNth(args)

	// Hole operations
	case "holes":
		return builtinHoles(args)
	case "ground?":
		return builtinGround(args)

	default:
		panic(fmt.Sprintf("unknown function: %s", name))
	}
}

// Arithmetic builtins

func builtinAdd(args []value.Value) value.Value {
	sum := 0.0
	for _, arg := range args {
		n, ok := arg.(value.Num)
		if !ok {
			panic(fmt.Sprintf("+ expects numbers, got %T", arg))
		}
		sum += n.Val
	}
	return value.NewNum(sum)
}

func builtinSub(args []value.Value) value.Value {
	if len(args) == 0 {
		panic("- requires at least one argument")
	}
	n, ok := args[0].(value.Num)
	if !ok {
		panic(fmt.Sprintf("- expects numbers, got %T", args[0]))
	}
	if len(args) == 1 {
		return value.NewNum(-n.Val)
	}
	result := n.Val
	for _, arg := range args[1:] {
		n, ok := arg.(value.Num)
		if !ok {
			panic(fmt.Sprintf("- expects numbers, got %T", arg))
		}
		result -= n.Val
	}
	return value.NewNum(result)
}

func builtinMul(args []value.Value) value.Value {
	product := 1.0
	for _, arg := range args {
		n, ok := arg.(value.Num)
		if !ok {
			panic(fmt.Sprintf("* expects numbers, got %T", arg))
		}
		product *= n.Val
	}
	return value.NewNum(product)
}

func builtinDiv(args []value.Value) value.Value {
	if len(args) == 0 {
		panic("/ requires at least one argument")
	}
	n, ok := args[0].(value.Num)
	if !ok {
		panic(fmt.Sprintf("/ expects numbers, got %T", args[0]))
	}
	if len(args) == 1 {
		if n.Val == 0 {
			panic("division by zero")
		}
		return value.NewNum(1 / n.Val)
	}
	result := n.Val
	for _, arg := range args[1:] {
		n, ok := arg.(value.Num)
		if !ok {
			panic(fmt.Sprintf("/ expects numbers, got %T", arg))
		}
		if n.Val == 0 {
			panic("division by zero")
		}
		result /= n.Val
	}
	return value.NewNum(result)
}

// Comparison builtins

func builtinEq(args []value.Value) value.Value {
	if len(args) != 2 {
		panic("= requires exactly 2 arguments")
	}
	n1, ok1 := args[0].(value.Num)
	n2, ok2 := args[1].(value.Num)
	if !ok1 || !ok2 {
		panic("= expects numbers")
	}
	return value.NewBool(n1.Val == n2.Val)
}

func builtinLt(args []value.Value) value.Value {
	if len(args) != 2 {
		panic("< requires exactly 2 arguments")
	}
	n1, ok1 := args[0].(value.Num)
	n2, ok2 := args[1].(value.Num)
	if !ok1 || !ok2 {
		panic("< expects numbers")
	}
	return value.NewBool(n1.Val < n2.Val)
}

func builtinGt(args []value.Value) value.Value {
	if len(args) != 2 {
		panic("> requires exactly 2 arguments")
	}
	n1, ok1 := args[0].(value.Num)
	n2, ok2 := args[1].(value.Num)
	if !ok1 || !ok2 {
		panic("> expects numbers")
	}
	return value.NewBool(n1.Val > n2.Val)
}

func builtinLe(args []value.Value) value.Value {
	if len(args) != 2 {
		panic("<= requires exactly 2 arguments")
	}
	n1, ok1 := args[0].(value.Num)
	n2, ok2 := args[1].(value.Num)
	if !ok1 || !ok2 {
		panic("<= expects numbers")
	}
	return value.NewBool(n1.Val <= n2.Val)
}

func builtinGe(args []value.Value) value.Value {
	if len(args) != 2 {
		panic(">= requires exactly 2 arguments")
	}
	n1, ok1 := args[0].(value.Num)
	n2, ok2 := args[1].(value.Num)
	if !ok1 || !ok2 {
		panic(">= expects numbers")
	}
	return value.NewBool(n1.Val >= n2.Val)
}

// Logic builtins

func builtinNot(args []value.Value) value.Value {
	if len(args) != 1 {
		panic("not requires exactly 1 argument")
	}
	return value.NewBool(!value.Truthy(args[0]))
}

// List builtins

func builtinList(args []value.Value) value.Value {
	return value.NewList(args...)
}

func builtinHead(args []value.Value) value.Value {
	if len(args) != 1 {
		panic("head requires exactly 1 argument")
	}
	l, ok := args[0].(value.List)
	if !ok {
		panic(fmt.Sprintf("head expects list, got %T", args[0]))
	}
	if len(l.Elements) == 0 {
		panic("head of empty list")
	}
	return l.Elements[0]
}

func builtinTail(args []value.Value) value.Value {
	if len(args) != 1 {
		panic("tail requires exactly 1 argument")
	}
	l, ok := args[0].(value.List)
	if !ok {
		panic(fmt.Sprintf("tail expects list, got %T", args[0]))
	}
	if len(l.Elements) == 0 {
		panic("tail of empty list")
	}
	return value.NewList(l.Elements[1:]...)
}

func builtinAppend(args []value.Value) value.Value {
	if len(args) != 2 {
		panic("append requires exactly 2 arguments")
	}
	l1, ok1 := args[0].(value.List)
	l2, ok2 := args[1].(value.List)
	if !ok1 || !ok2 {
		panic("append expects lists")
	}
	combined := make([]value.Value, 0, len(l1.Elements)+len(l2.Elements))
	combined = append(combined, l1.Elements...)
	combined = append(combined, l2.Elements...)
	return value.NewList(combined...)
}

func builtinLength(args []value.Value) value.Value {
	if len(args) != 1 {
		panic("length requires exactly 1 argument")
	}
	l, ok := args[0].(value.List)
	if !ok {
		panic(fmt.Sprintf("length expects list, got %T", args[0]))
	}
	return value.NewNum(float64(len(l.Elements)))
}

func builtinEmpty(args []value.Value) value.Value {
	if len(args) != 1 {
		panic("empty? requires exactly 1 argument")
	}
	l, ok := args[0].(value.List)
	if !ok {
		panic(fmt.Sprintf("empty? expects list, got %T", args[0]))
	}
	return value.NewBool(len(l.Elements) == 0)
}

func builtinNth(args []value.Value) value.Value {
	if len(args) != 2 {
		panic("nth requires exactly 2 arguments")
	}
	l, ok1 := args[0].(value.List)
	n, ok2 := args[1].(value.Num)
	if !ok1 || !ok2 {
		panic("nth expects (list number)")
	}
	idx := int(n.Val)
	if idx < 0 || idx >= len(l.Elements) {
		panic(fmt.Sprintf("nth index out of bounds: %d", idx))
	}
	return l.Elements[idx]
}

// Hole builtins

func builtinHoles(args []value.Value) value.Value {
	// holes operates on quoted expressions (values)
	if len(args) != 1 {
		panic("holes requires exactly 1 argument")
	}
	holes := collectValueHoles(args[0])
	vals := make([]value.Value, len(holes))
	for i, h := range holes {
		vals[i] = value.NewStr(h)
	}
	return value.NewList(vals...)
}

func builtinGround(args []value.Value) value.Value {
	if len(args) != 1 {
		panic("ground? requires exactly 1 argument")
	}
	return value.NewBool(!hasValueHoles(args[0]))
}

// Helper functions

func exprToValue(expr ast.Expr) value.Value {
	switch x := expr.(type) {
	case *ast.AtomExpr:
		switch x.Kind {
		case ast.AtomNil:
			return value.NewNil()
		case ast.AtomNum:
			return value.NewNum(x.NumVal)
		case ast.AtomStr:
			return value.NewStr(x.StrVal)
		case ast.AtomBool:
			return value.NewBool(x.BoolVal)
		case ast.AtomSym:
			return value.NewSym(x.StrVal)
		case ast.AtomHole:
			return value.NewNamespacedHole(x.Namespace, x.StrVal)
		}
	case *ast.ListExpr:
		vals := make([]value.Value, len(x.Elements))
		for i, e := range x.Elements {
			vals[i] = exprToValue(e)
		}
		return value.NewList(vals...)
	case *ast.QuoteExpr:
		// Quote of quote becomes list (quote inner)
		inner := exprToValue(x.Inner)
		return value.NewList(value.NewSym("quote"), inner)
	}
	panic(fmt.Sprintf("cannot convert expression to value: %T", expr))
}

func collectValueHoles(v value.Value) []string {
	var holes []string
	seen := make(map[string]bool)

	var walk func(value.Value)
	walk = func(v value.Value) {
		switch x := v.(type) {
		case value.Hole:
			name := x.FullName()
			if !seen[name] {
				seen[name] = true
				holes = append(holes, name)
			}
		case value.List:
			for _, elem := range x.Elements {
				walk(elem)
			}
		}
	}
	walk(v)
	return holes
}

func hasValueHoles(v value.Value) bool {
	switch x := v.(type) {
	case value.Hole:
		return true
	case value.List:
		for _, elem := range x.Elements {
			if hasValueHoles(elem) {
				return true
			}
		}
	}
	return false
}
