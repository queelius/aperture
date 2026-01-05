// Package env provides lexically scoped environments for the apertures language.
package env

import (
	"github.com/queelius/aperture/pkg/value"
)

// Env represents a lexically scoped environment.
// Each environment has an optional parent, forming a chain for scope lookup.
type Env struct {
	bindings map[string]value.Value
	parent   *Env
}

// New creates a new empty environment with no parent.
func New() *Env {
	return &Env{
		bindings: make(map[string]value.Value),
		parent:   nil,
	}
}

// NewWithParent creates a new environment with the given parent.
func NewWithParent(parent *Env) *Env {
	return &Env{
		bindings: make(map[string]value.Value),
		parent:   parent,
	}
}

// Extend creates a child environment with the given bindings.
// This is typically used for function calls and let bindings.
func (e *Env) Extend(names []string, values []value.Value) *Env {
	child := NewWithParent(e)
	for i, name := range names {
		if i < len(values) {
			child.bindings[name] = values[i]
		}
	}
	return child
}

// Define adds or updates a binding in the current environment.
// This does not affect parent environments.
func (e *Env) Define(name string, val value.Value) {
	e.bindings[name] = val
}

// Set updates an existing binding, searching up the parent chain.
// Panics if the binding doesn't exist.
func (e *Env) Set(name string, val value.Value) {
	env := e
	for env != nil {
		if _, ok := env.bindings[name]; ok {
			env.bindings[name] = val
			return
		}
		env = env.parent
	}
	panic("undefined variable: " + name)
}

// Lookup searches for a binding by name, traversing parent environments.
// Returns the value and true if found, or nil and false if not found.
func (e *Env) Lookup(name string) (value.Value, bool) {
	env := e
	for env != nil {
		if val, ok := env.bindings[name]; ok {
			return val, true
		}
		env = env.parent
	}
	return nil, false
}

// MustLookup is like Lookup but panics if the binding is not found.
func (e *Env) MustLookup(name string) value.Value {
	if val, ok := e.Lookup(name); ok {
		return val
	}
	panic("undefined variable: " + name)
}

// Has returns true if the name is bound in this environment or any parent.
func (e *Env) Has(name string) bool {
	_, ok := e.Lookup(name)
	return ok
}

// LocalHas returns true if the name is bound in this environment (not parents).
func (e *Env) LocalHas(name string) bool {
	_, ok := e.bindings[name]
	return ok
}

// Names returns all bound names in this environment (not parents).
func (e *Env) Names() []string {
	names := make([]string, 0, len(e.bindings))
	for name := range e.bindings {
		names = append(names, name)
	}
	return names
}

// AllNames returns all bound names in this environment and all parents.
func (e *Env) AllNames() []string {
	seen := make(map[string]bool)
	var names []string
	env := e
	for env != nil {
		for name := range env.bindings {
			if !seen[name] {
				seen[name] = true
				names = append(names, name)
			}
		}
		env = env.parent
	}
	return names
}

// Parent returns the parent environment, or nil if this is the root.
func (e *Env) Parent() *Env {
	return e.parent
}

// Clone creates a shallow copy of this environment.
// The bindings map is copied, but parent reference is shared.
func (e *Env) Clone() *Env {
	bindings := make(map[string]value.Value, len(e.bindings))
	for k, v := range e.bindings {
		bindings[k] = v
	}
	return &Env{
		bindings: bindings,
		parent:   e.parent,
	}
}

// Global creates a new environment with standard built-in values.
func Global() *Env {
	env := New()
	// Pre-defined constants
	env.Define("nil", value.NewNil())
	env.Define("true", value.NewBool(true))
	env.Define("false", value.NewBool(false))
	return env
}
