package eval

import (
	"testing"

	"github.com/queelius/aperture/pkg/env"
	"github.com/queelius/aperture/pkg/parser"
	"github.com/queelius/aperture/pkg/value"
)

func TestArithmetic(t *testing.T) {
	tests := []struct {
		input    string
		expected float64
	}{
		{"(+ 3 5)", 8},
		{"(- 10 3)", 7},
		{"(* 6 7)", 42},
		{"(/ 20 4)", 5},
		{"(+ 1 2 3 4)", 10},
		{"(* 2 3 4)", 24},
		{"(- 5)", -5},
		{"(/ 2)", 0.5},
	}

	for _, tt := range tests {
		t.Run(tt.input, func(t *testing.T) {
			expr, err := parser.ParseOne(tt.input)
			if err != nil {
				t.Fatalf("parse error: %v", err)
			}
			e := env.Global()
			result := Eval(expr, e)
			num, ok := result.(value.Num)
			if !ok {
				t.Fatalf("expected Num, got %T", result)
			}
			if num.Val != tt.expected {
				t.Errorf("expected %v, got %v", tt.expected, num.Val)
			}
		})
	}
}

func TestLambda(t *testing.T) {
	tests := []struct {
		input    string
		expected float64
	}{
		{"((lambda (x) (+ x 1)) 5)", 6},
		{"((lambda (x y) (+ x y)) 3 4)", 7},
		{"((lambda (x) (* x x)) 5)", 25},
	}

	for _, tt := range tests {
		t.Run(tt.input, func(t *testing.T) {
			expr, err := parser.ParseOne(tt.input)
			if err != nil {
				t.Fatalf("parse error: %v", err)
			}
			e := env.Global()
			result := Eval(expr, e)
			num, ok := result.(value.Num)
			if !ok {
				t.Fatalf("expected Num, got %T", result)
			}
			if num.Val != tt.expected {
				t.Errorf("expected %v, got %v", tt.expected, num.Val)
			}
		})
	}
}

func TestLet(t *testing.T) {
	tests := []struct {
		input    string
		expected float64
	}{
		{"(let ((x 10)) x)", 10},
		{"(let ((x 10) (y 20)) (+ x y))", 30},
		{"(let ((x 5)) (let ((y 10)) (+ x y)))", 15},
	}

	for _, tt := range tests {
		t.Run(tt.input, func(t *testing.T) {
			expr, err := parser.ParseOne(tt.input)
			if err != nil {
				t.Fatalf("parse error: %v", err)
			}
			e := env.Global()
			result := Eval(expr, e)
			num, ok := result.(value.Num)
			if !ok {
				t.Fatalf("expected Num, got %T", result)
			}
			if num.Val != tt.expected {
				t.Errorf("expected %v, got %v", tt.expected, num.Val)
			}
		})
	}
}

func TestConditional(t *testing.T) {
	tests := []struct {
		input    string
		expected float64
	}{
		{"(if true 1 2)", 1},
		{"(if false 1 2)", 2},
		{"(if (> 5 3) 100 200)", 100},
		{"(if (< 5 3) 100 200)", 200},
	}

	for _, tt := range tests {
		t.Run(tt.input, func(t *testing.T) {
			expr, err := parser.ParseOne(tt.input)
			if err != nil {
				t.Fatalf("parse error: %v", err)
			}
			e := env.Global()
			result := Eval(expr, e)
			num, ok := result.(value.Num)
			if !ok {
				t.Fatalf("expected Num, got %T", result)
			}
			if num.Val != tt.expected {
				t.Errorf("expected %v, got %v", tt.expected, num.Val)
			}
		})
	}
}

func TestPartialEval(t *testing.T) {
	tests := []struct {
		input    string
		expected string
	}{
		{"(+ 3 ?x 5)", "(+ 8 ?x)"},
		{"(* 0 ?anything)", "0"},
		{"(* 1 ?x)", "?x"},
		{"(+ 0 ?x)", "?x"},
		{"(if true ?x ?y)", "?x"},
		{"(if false ?x ?y)", "?y"},
	}

	for _, tt := range tests {
		t.Run(tt.input, func(t *testing.T) {
			expr, err := parser.ParseOne(tt.input)
			if err != nil {
				t.Fatalf("parse error: %v", err)
			}
			e := env.Global()
			result := PartialEval(expr, e)
			if result.String() != tt.expected {
				t.Errorf("expected %s, got %s", tt.expected, result.String())
			}
		})
	}
}

func TestFill(t *testing.T) {
	tests := []struct {
		input    string
		bindings map[string]value.Value
		expected float64
	}{
		{"(+ ?x 5)", map[string]value.Value{"x": value.NewNum(10)}, 15},
		{"(* ?x ?y)", map[string]value.Value{"x": value.NewNum(3), "y": value.NewNum(4)}, 12},
	}

	for _, tt := range tests {
		t.Run(tt.input, func(t *testing.T) {
			expr, err := parser.ParseOne(tt.input)
			if err != nil {
				t.Fatalf("parse error: %v", err)
			}
			e := env.Global()
			filled := Fill(expr, tt.bindings)
			result := Eval(filled, e)
			num, ok := result.(value.Num)
			if !ok {
				t.Fatalf("expected Num, got %T", result)
			}
			if num.Val != tt.expected {
				t.Errorf("expected %v, got %v", tt.expected, num.Val)
			}
		})
	}
}

func TestHoles(t *testing.T) {
	tests := []struct {
		input    string
		expected []string
	}{
		{"(+ ?x 5)", []string{"x"}},
		{"(+ ?x ?y)", []string{"x", "y"}},
		{"(+ 1 2)", nil},
		{"(+ ?x ?x)", []string{"x"}}, // Same hole twice
		{"(+ ?client.x ?server.y)", []string{"client.x", "server.y"}},
	}

	for _, tt := range tests {
		t.Run(tt.input, func(t *testing.T) {
			expr, err := parser.ParseOne(tt.input)
			if err != nil {
				t.Fatalf("parse error: %v", err)
			}
			holes := Holes(expr)
			if len(holes) != len(tt.expected) {
				t.Errorf("expected %d holes, got %d", len(tt.expected), len(holes))
				return
			}
			for i, h := range holes {
				if h != tt.expected[i] {
					t.Errorf("expected hole %s, got %s", tt.expected[i], h)
				}
			}
		})
	}
}

func TestDefine(t *testing.T) {
	e := env.Global()

	// Define a simple value
	expr1, _ := parser.ParseOne("(define x 10)")
	Eval(expr1, e)

	// Use it
	expr2, _ := parser.ParseOne("(+ x 5)")
	result := Eval(expr2, e)
	num := result.(value.Num)
	if num.Val != 15 {
		t.Errorf("expected 15, got %v", num.Val)
	}

	// Define a function
	expr3, _ := parser.ParseOne("(define (double n) (* n 2))")
	Eval(expr3, e)

	// Use it
	expr4, _ := parser.ParseOne("(double 21)")
	result = Eval(expr4, e)
	num = result.(value.Num)
	if num.Val != 42 {
		t.Errorf("expected 42, got %v", num.Val)
	}
}
