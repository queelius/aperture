// Command server is an HTTP server that accepts aperture expressions,
// partially evaluates them, and returns the optimized result.
package main

import (
	"encoding/json"
	"log"
	"net/http"

	"github.com/queelius/aperture/pkg/env"
	"github.com/queelius/aperture/pkg/eval"
	"github.com/queelius/aperture/pkg/parser"
	"github.com/queelius/aperture/pkg/trace"
)

// OptimizeRequest is the request body for /optimize
type OptimizeRequest struct {
	Expression string `json:"expression"`
}

// OptimizeResponse is the response body for /optimize
type OptimizeResponse struct {
	Optimized     string   `json:"optimized"`
	UnfilledHoles []string `json:"unfilled_holes"`
	Trace         []trace.Step `json:"trace,omitempty"`
	Error         string   `json:"error,omitempty"`
}

func main() {
	http.HandleFunc("/optimize", handleOptimize)
	http.HandleFunc("/health", handleHealth)

	addr := ":8080"
	log.Printf("Aperture optimization server listening on %s", addr)
	log.Fatal(http.ListenAndServe(addr, nil))
}

func handleHealth(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(map[string]string{"status": "ok"})
}

func handleOptimize(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
		return
	}

	var req OptimizeRequest
	if err := json.NewDecoder(r.Body).Decode(&req); err != nil {
		respondError(w, "Invalid JSON: "+err.Error())
		return
	}

	if req.Expression == "" {
		respondError(w, "Expression is required")
		return
	}

	// Parse the expression
	expr, err := parser.ParseOne(req.Expression)
	if err != nil {
		respondError(w, "Parse error: "+err.Error())
		return
	}

	// Create a fresh environment
	e := env.Global()

	// Partially evaluate
	optimized := eval.PartialEval(expr, e)

	// Collect holes
	holes := eval.Holes(optimized)

	// Build response
	resp := OptimizeResponse{
		Optimized:     optimized.String(),
		UnfilledHoles: holes,
	}

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(resp)
}

func respondError(w http.ResponseWriter, msg string) {
	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(http.StatusBadRequest)
	json.NewEncoder(w).Encode(OptimizeResponse{Error: msg})
}

// Usage example:
//
// curl -X POST http://localhost:8080/optimize \
//   -H "Content-Type: application/json" \
//   -d '{"expression": "(+ 3 ?x 5)"}'
//
// Response:
// {"optimized": "(+ 8 ?x)", "unfilled_holes": ["x"]}
