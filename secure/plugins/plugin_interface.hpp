#pragma once
#include "../aperture.hpp"
#include <memory>
#include <vector>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>

namespace aperture::plugins {

// Forward declarations
class EvaluationContext;
class Plugin;
using PluginPtr = std::shared_ptr<Plugin>;

// Context provided to plugins during evaluation
class EvaluationContext {
public:
    // The expression being evaluated
    secure::ValuePtr expression;
    
    // Current environment bindings
    std::unordered_map<std::string, secure::ValuePtr> environment;
    
    // Holes found in the expression
    std::vector<std::string> holes;
    
    // Parent expressions (for context)
    std::vector<secure::ValuePtr> parent_stack;
    
    // Metadata about the computation
    struct Meta {
        std::string description;       // User-provided description
        std::string domain;            // Domain hint (e.g., "finance", "medical")
        double confidence_threshold;   // Minimum confidence for hole filling
        bool allow_approximation;      // Whether approximate values are acceptable
        std::unordered_map<std::string, std::string> hints; // Hints about holes
    } meta;
    
    // Add a parent to the context stack
    void push_parent(secure::ValuePtr parent) {
        parent_stack.push_back(parent);
    }
    
    void pop_parent() {
        if (!parent_stack.empty()) {
            parent_stack.pop_back();
        }
    }
    
    // Get the full expression as a string for context
    std::string get_full_context() const {
        return secure::to_string(expression);
    }
    
    // Get surrounding context for a specific hole
    std::string get_hole_context(const std::string& hole_name) const {
        // This would extract the local context around the hole
        return get_full_context(); // Simplified for now
    }
};

// Result from a plugin's attempt to fill a hole
struct HoleFillResult {
    std::string hole_name;
    secure::ValuePtr value;
    double confidence;  // 0.0 to 1.0
    std::string explanation;
    std::string source;  // Where the value came from
};

// Base plugin interface
class Plugin {
public:
    virtual ~Plugin() = default;
    
    // Plugin metadata
    virtual std::string name() const = 0;
    virtual std::string description() const = 0;
    virtual std::string version() const = 0;
    
    // Called when a hole is encountered during evaluation
    virtual std::optional<HoleFillResult> suggest_hole_value(
        const std::string& hole_name,
        const EvaluationContext& context
    ) = 0;
    
    // Called before evaluation starts
    virtual void on_evaluation_start(const EvaluationContext& context) {}
    
    // Called after evaluation completes
    virtual void on_evaluation_complete(
        const EvaluationContext& context,
        secure::ValuePtr result
    ) {}
    
    // Check if plugin can handle this type of computation
    virtual bool can_handle(const EvaluationContext& context) {
        return true; // By default, handle everything
    }
    
    // Priority for this plugin (higher = evaluated first)
    virtual int priority() const { return 0; }
};

// Plugin that uses pattern matching to fill holes
class PatternMatchPlugin : public Plugin {
    struct Pattern {
        std::string pattern_expr;  // Expression pattern to match
        std::unordered_map<std::string, double> hole_values;  // Values to use
        std::string explanation;
    };
    
    std::vector<Pattern> patterns;
    
public:
    std::string name() const override { return "PatternMatch"; }
    std::string description() const override { 
        return "Fills holes based on pattern matching"; 
    }
    std::string version() const override { return "1.0.0"; }
    
    void add_pattern(const std::string& pattern, 
                     const std::unordered_map<std::string, double>& values,
                     const std::string& explanation) {
        patterns.push_back({pattern, values, explanation});
    }
    
    std::optional<HoleFillResult> suggest_hole_value(
        const std::string& hole_name,
        const EvaluationContext& context
    ) override {
        // Simple pattern matching implementation
        for (const auto& pattern : patterns) {
            auto it = pattern.hole_values.find(hole_name);
            if (it != pattern.hole_values.end()) {
                return HoleFillResult{
                    hole_name,
                    secure::num(it->second),
                    0.8,  // Fixed confidence for pattern matches
                    pattern.explanation,
                    "pattern_match"
                };
            }
        }
        return std::nullopt;
    }
};

// Plugin that uses mathematical context to infer values
class MathContextPlugin : public Plugin {
public:
    std::string name() const override { return "MathContext"; }
    std::string description() const override { 
        return "Infers mathematical constants and common values"; 
    }
    std::string version() const override { return "1.0.0"; }
    
    std::optional<HoleFillResult> suggest_hole_value(
        const std::string& hole_name,
        const EvaluationContext& context
    ) override {
        // Common mathematical constants
        static const std::unordered_map<std::string, double> constants = {
            {"pi", 3.14159265359},
            {"e", 2.71828182846},
            {"phi", 1.61803398875},  // Golden ratio
            {"sqrt2", 1.41421356237},
            {"sqrt3", 1.73205080757}
        };
        
        // Check if hole name matches a known constant
        auto it = constants.find(hole_name);
        if (it != constants.end()) {
            return HoleFillResult{
                hole_name,
                secure::num(it->second),
                0.99,  // High confidence for mathematical constants
                "Mathematical constant " + hole_name,
                "math_constants"
            };
        }
        
        // Check for common variable patterns
        if (hole_name == "zero" || hole_name == "Z") {
            return HoleFillResult{
                hole_name,
                secure::num(0),
                0.9,
                "Common zero value",
                "math_inference"
            };
        }
        
        if (hole_name == "one" || hole_name == "unity") {
            return HoleFillResult{
                hole_name,
                secure::num(1),
                0.9,
                "Unity/identity value",
                "math_inference"
            };
        }
        
        return std::nullopt;
    }
};

// Plugin manager singleton
class PluginManager {
    std::vector<PluginPtr> plugins;
    bool auto_fill_enabled = false;
    double min_confidence = 0.7;
    
    PluginManager() = default;
    
public:
    static PluginManager& instance() {
        static PluginManager mgr;
        return mgr;
    }
    
    // Register a plugin
    void register_plugin(PluginPtr plugin) {
        plugins.push_back(plugin);
        // Sort by priority
        std::sort(plugins.begin(), plugins.end(),
                 [](const PluginPtr& a, const PluginPtr& b) {
                     return a->priority() > b->priority();
                 });
    }
    
    // Enable/disable automatic hole filling
    void set_auto_fill(bool enabled) {
        auto_fill_enabled = enabled;
    }
    
    bool is_auto_fill_enabled() const {
        return auto_fill_enabled;
    }
    
    // Set minimum confidence threshold
    void set_min_confidence(double confidence) {
        min_confidence = confidence;
    }
    
    // Try to fill a hole using plugins
    std::optional<HoleFillResult> try_fill_hole(
        const std::string& hole_name,
        const EvaluationContext& context
    ) {
        if (!auto_fill_enabled) {
            return std::nullopt;
        }
        
        for (const auto& plugin : plugins) {
            if (!plugin->can_handle(context)) {
                continue;
            }
            
            auto result = plugin->suggest_hole_value(hole_name, context);
            if (result && result->confidence >= min_confidence) {
                return result;
            }
        }
        
        return std::nullopt;
    }
    
    // Notify plugins of evaluation start
    void notify_evaluation_start(const EvaluationContext& context) {
        for (const auto& plugin : plugins) {
            plugin->on_evaluation_start(context);
        }
    }
    
    // Notify plugins of evaluation complete
    void notify_evaluation_complete(const EvaluationContext& context,
                                   secure::ValuePtr result) {
        for (const auto& plugin : plugins) {
            plugin->on_evaluation_complete(context, result);
        }
    }
    
    // List registered plugins
    std::vector<std::string> list_plugins() const {
        std::vector<std::string> names;
        for (const auto& plugin : plugins) {
            names.push_back(plugin->name() + " v" + plugin->version());
        }
        return names;
    }
    
    // Clear all plugins
    void clear() {
        plugins.clear();
    }
};

} // namespace aperture::plugins