// Command client demonstrates the aperture client-server workflow.
// It sends an expression to the server for optimization, then fills
// holes locally and evaluates the result.
package main

import (
	"bytes"
	"encoding/json"
	"flag"
	"fmt"
	"io"
	"net/http"
	"os"
	"strconv"
	"strings"

	"github.com/queelius/aperture/pkg/env"
	"github.com/queelius/aperture/pkg/eval"
	"github.com/queelius/aperture/pkg/parser"
	"github.com/queelius/aperture/pkg/value"
)

type OptimizeRequest struct {
	Expression string `json:"expression"`
}

type OptimizeResponse struct {
	Optimized     string   `json:"optimized"`
	UnfilledHoles []string `json:"unfilled_holes"`
	Error         string   `json:"error,omitempty"`
}

func main() {
	serverURL := flag.String("server", "http://localhost:8080", "Server URL")
	flag.Parse()

	if flag.NArg() < 1 {
		fmt.Println("Usage: client [-server URL] '<expression>' [hole=value ...]")
		fmt.Println()
		fmt.Println("Example:")
		fmt.Println("  client '(+ 3 ?x 5)' x=10")
		fmt.Println()
		fmt.Println("This sends the expression to the server for optimization,")
		fmt.Println("then fills the holes locally and evaluates.")
		os.Exit(1)
	}

	expression := flag.Arg(0)

	// Parse hole bindings from remaining arguments
	bindings := make(map[string]value.Value)
	for i := 1; i < flag.NArg(); i++ {
		arg := flag.Arg(i)
		parts := strings.SplitN(arg, "=", 2)
		if len(parts) != 2 {
			fmt.Fprintf(os.Stderr, "Invalid binding format: %s (expected name=value)\n", arg)
			os.Exit(1)
		}
		name := parts[0]
		valStr := parts[1]

		// Parse value
		if f, err := strconv.ParseFloat(valStr, 64); err == nil {
			bindings[name] = value.NewNum(f)
		} else if valStr == "true" {
			bindings[name] = value.NewBool(true)
		} else if valStr == "false" {
			bindings[name] = value.NewBool(false)
		} else if valStr == "nil" {
			bindings[name] = value.NewNil()
		} else {
			bindings[name] = value.NewStr(valStr)
		}
	}

	// Step 1: Send to server for optimization
	fmt.Println("=== Step 1: Send to server for optimization ===")
	fmt.Printf("Original: %s\n", expression)

	optimized, holes, err := sendToServer(*serverURL, expression)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Server error: %v\n", err)
		os.Exit(1)
	}

	fmt.Printf("Optimized: %s\n", optimized)
	fmt.Printf("Unfilled holes: %v\n", holes)
	fmt.Println()

	// Step 2: Fill holes locally
	fmt.Println("=== Step 2: Fill holes locally ===")
	if len(bindings) == 0 {
		fmt.Println("No bindings provided. Stopping here.")
		return
	}

	for name, val := range bindings {
		fmt.Printf("  %s = %s\n", name, val)
	}
	fmt.Println()

	// Parse the optimized expression
	expr, err := parser.ParseOne(optimized)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Parse error: %v\n", err)
		os.Exit(1)
	}

	// Fill holes
	filled := eval.Fill(expr, bindings)
	fmt.Printf("After fill: %s\n", filled)
	fmt.Println()

	// Step 3: Evaluate locally
	fmt.Println("=== Step 3: Evaluate locally ===")

	e := env.Global()
	finalExpr := eval.PartialEval(filled, e)

	if eval.IsGround(finalExpr) {
		result := safeEval(finalExpr, e)
		if result != nil {
			fmt.Printf("Result: %s\n", result)
		}
	} else {
		fmt.Printf("Result (with remaining holes): %s\n", finalExpr)
		fmt.Printf("Remaining holes: %v\n", eval.Holes(finalExpr))
	}
}

func sendToServer(serverURL, expression string) (string, []string, error) {
	req := OptimizeRequest{Expression: expression}
	body, err := json.Marshal(req)
	if err != nil {
		return "", nil, err
	}

	resp, err := http.Post(serverURL+"/optimize", "application/json", bytes.NewReader(body))
	if err != nil {
		return "", nil, err
	}
	defer resp.Body.Close()

	respBody, err := io.ReadAll(resp.Body)
	if err != nil {
		return "", nil, err
	}

	var result OptimizeResponse
	if err := json.Unmarshal(respBody, &result); err != nil {
		return "", nil, err
	}

	if result.Error != "" {
		return "", nil, fmt.Errorf("%s", result.Error)
	}

	return result.Optimized, result.UnfilledHoles, nil
}

func safeEval(expr interface{}, e *env.Env) (result value.Value) {
	defer func() {
		if r := recover(); r != nil {
			fmt.Fprintf(os.Stderr, "Evaluation error: %v\n", r)
			result = nil
		}
	}()

	// Type assert to ast.Expr
	if astExpr, ok := expr.(interface{ String() string }); ok {
		// Re-parse to get proper AST type
		parsed, err := parser.ParseOne(astExpr.String())
		if err != nil {
			return nil
		}
		return eval.Eval(parsed, e)
	}
	return nil
}
