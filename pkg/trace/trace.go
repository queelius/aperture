// Package trace provides structured tracing for aperture evaluation.
package trace

import (
	"encoding/json"
	"time"
)

// Trace represents a complete evaluation trace.
type Trace struct {
	Steps        []Step   `json:"trace"`
	Result       string   `json:"result"`
	UnfilledHoles []string `json:"unfilled_holes,omitempty"`
}

// Step represents a single evaluation step.
type Step struct {
	StepNum int    `json:"step"`
	Rule    string `json:"rule"`
	Before  string `json:"before"`
	After   string `json:"after"`
}

// FillEvent records a hole fill operation with principal tracking.
type FillEvent struct {
	Principal string    `json:"principal"`
	Hole      string    `json:"hole"`
	Value     string    `json:"value"`
	Timestamp time.Time `json:"timestamp"`
}

// FillLog records all fill events.
type FillLog struct {
	Events []FillEvent `json:"events"`
}

// Tracer collects evaluation steps.
type Tracer struct {
	steps   []Step
	stepNum int
}

// NewTracer creates a new tracer.
func NewTracer() *Tracer {
	return &Tracer{
		steps:   make([]Step, 0),
		stepNum: 0,
	}
}

// Record adds a step to the trace.
func (t *Tracer) Record(rule, before, after string) {
	t.stepNum++
	t.steps = append(t.steps, Step{
		StepNum: t.stepNum,
		Rule:    rule,
		Before:  before,
		After:   after,
	})
}

// Steps returns all recorded steps.
func (t *Tracer) Steps() []Step {
	return t.steps
}

// ToTrace creates a Trace from the recorded steps.
func (t *Tracer) ToTrace(result string, holes []string) Trace {
	return Trace{
		Steps:        t.steps,
		Result:       result,
		UnfilledHoles: holes,
	}
}

// JSON returns the trace as a JSON string.
func (tr Trace) JSON() string {
	data, err := json.MarshalIndent(tr, "", "  ")
	if err != nil {
		return "{}"
	}
	return string(data)
}

// NewFillLog creates a new fill event log.
func NewFillLog() *FillLog {
	return &FillLog{Events: make([]FillEvent, 0)}
}

// Record adds a fill event to the log.
func (fl *FillLog) Record(principal, hole, value string) {
	fl.Events = append(fl.Events, FillEvent{
		Principal: principal,
		Hole:      hole,
		Value:     value,
		Timestamp: time.Now(),
	})
}

// JSON returns the fill log as a JSON string.
func (fl *FillLog) JSON() string {
	data, err := json.MarshalIndent(fl, "", "  ")
	if err != nil {
		return "{}"
	}
	return string(data)
}

// Common rule names for tracing
const (
	RuleConstFold     = "const-fold"
	RuleIdentity      = "identity"
	RuleAnnihilation  = "annihilation"
	RuleCondElim      = "cond-elim"
	RuleLookup        = "lookup"
	RuleApply         = "apply"
	RuleFill          = "fill"
	RuleSimplify      = "simplify"
)
