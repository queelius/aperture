#include "aperture.hpp"
#include "plugins/plugin_interface.hpp"
#include "plugins/llm_plugin.hpp"
#include <iostream>
#include <iomanip>

using namespace aperture::plugins;
using namespace secure;

void demo_math_context_plugin() {
    std::cout << "\n=== Math Context Plugin Demo ===\n";
    
    // Register math context plugin
    auto math_plugin = std::make_shared<MathContextPlugin>();
    PluginManager::instance().register_plugin(math_plugin);
    PluginManager::instance().set_auto_fill(true);
    
    // Expression with mathematical constant
    Parser parser("(* ?pi (* ?r ?r))");  // Area = πr²
    auto expr = parser.parse();
    if (!expr) {
        std::cerr << "Parse error\n";
        return;
    }
    
    std::cout << "Original expression: " << to_string(*expr) << "\n";
    
    // Create evaluation context
    EvaluationContext context;
    context.expression = *expr;
    context.holes = {"pi", "r"};
    
    // Try to fill pi
    auto result = PluginManager::instance().try_fill_hole("pi", context);
    if (result) {
        std::cout << "Plugin filled 'pi' with: " << result->confidence 
                  << " confidence\n";
        std::cout << "Explanation: " << result->explanation << "\n";
        
        // Fill the hole and evaluate with r=5
        Evaluator eval;
        std::unordered_map<std::string, ValuePtr> values;
        values["pi"] = result->value;
        values["r"] = num(5);
        
        auto filled = eval.fill_holes(*expr, values);
        if (filled) {
            auto final_result = eval.eval(*filled);
            if (final_result) {
                if (auto* n = std::get_if<Value::Num>(&(*final_result)->data)) {
                    std::cout << "Area of circle with r=5: " << n->val << "\n";
                }
            }
        }
    }
    
    PluginManager::instance().clear();
}

void demo_llm_plugin() {
    std::cout << "\n=== LLM Plugin Demo ===\n";
    
    // Register LLM plugin with verbose output
    auto llm_plugin = std::make_shared<LLMPlugin>(nullptr, true);
    PluginManager::instance().register_plugin(llm_plugin);
    PluginManager::instance().set_auto_fill(true);
    PluginManager::instance().set_min_confidence(0.5);
    
    // Financial calculation with domain context
    std::string formula = "(* ?portfolio_value (+ 1 ?interest_rate))";
    Parser parser(formula);
    auto expr = parser.parse();
    if (!expr) {
        std::cerr << "Parse error\n";
        return;
    }
    
    std::cout << "Financial formula: " << to_string(*expr) << "\n\n";
    
    // Create context with domain hint
    EvaluationContext context;
    context.expression = *expr;
    context.holes = {"portfolio_value", "interest_rate"};
    context.meta.domain = "finance";
    context.meta.description = "Calculate portfolio value after interest";
    context.meta.confidence_threshold = 0.5;
    
    // Try to fill interest_rate using LLM
    auto rate_result = PluginManager::instance().try_fill_hole("interest_rate", context);
    if (rate_result) {
        std::cout << "\nLLM suggested interest_rate = " 
                  << std::get_if<Value::Num>(&rate_result->value->data)->val
                  << " with confidence " << rate_result->confidence << "\n";
        
        // Complete calculation with portfolio value
        Evaluator eval;
        std::unordered_map<std::string, ValuePtr> values;
        values["interest_rate"] = rate_result->value;
        values["portfolio_value"] = num(10000);
        
        auto filled = eval.fill_holes(*expr, values);
        if (filled) {
            auto result = eval.eval(*filled);
            if (result && std::get_if<Value::Num>(&(*result)->data)) {
                std::cout << "Portfolio value after interest: " 
                         << std::get_if<Value::Num>(&(*result)->data)->val << "\n";
            }
        }
    }
    
    PluginManager::instance().clear();
}

void demo_pattern_matching() {
    std::cout << "\n=== Pattern Matching Plugin Demo ===\n";
    
    auto pattern_plugin = std::make_shared<PatternMatchPlugin>();
    
    // Add patterns for common formulas
    pattern_plugin->add_pattern(
        "quadratic",
        {{"a", 1}, {"b", 0}, {"c", 0}},
        "Standard quadratic form defaults"
    );
    
    pattern_plugin->add_pattern(
        "physics",
        {{"g", 9.81}, {"c", 299792458}},
        "Physical constants"
    );
    
    PluginManager::instance().register_plugin(pattern_plugin);
    PluginManager::instance().set_auto_fill(true);
    
    // Physics formula: distance = 0.5 * g * t²
    Parser parser("(* 0.5 (* ?g (* ?t ?t)))");
    auto expr = parser.parse();
    if (!expr) return;
    
    std::cout << "Physics formula: " << to_string(*expr) << "\n";
    
    EvaluationContext context;
    context.expression = *expr;
    context.holes = {"g", "t"};
    
    // Try to fill g
    auto g_result = PluginManager::instance().try_fill_hole("g", context);
    if (g_result) {
        std::cout << "Pattern plugin filled 'g' = " 
                  << std::get_if<Value::Num>(&g_result->value->data)->val << "\n";
        
        // Calculate with t=2 seconds
        Evaluator eval;
        std::unordered_map<std::string, ValuePtr> values;
        values["g"] = g_result->value;
        values["t"] = num(2);
        
        auto filled = eval.fill_holes(*expr, values);
        if (filled) {
            auto result = eval.eval(*filled);
            if (result && std::get_if<Value::Num>(&(*result)->data)) {
                std::cout << "Distance fallen in 2 seconds: " 
                         << std::get_if<Value::Num>(&(*result)->data)->val << " meters\n";
            }
        }
    }
    
    PluginManager::instance().clear();
}

void demo_medical_context() {
    std::cout << "\n=== Medical Context with LLM ===\n";
    
    auto llm_plugin = std::make_shared<LLMPlugin>(nullptr, true);
    PluginManager::instance().register_plugin(llm_plugin);
    PluginManager::instance().set_auto_fill(true);
    PluginManager::instance().set_min_confidence(0.5);
    
    // Medical dosage calculation
    std::string formula = "(if (> ?dosage ?max_dose) ?max_dose ?dosage)";
    Parser parser(formula);
    auto expr = parser.parse();
    if (!expr) return;
    
    std::cout << "Dosage safety check: " << to_string(*expr) << "\n\n";
    
    EvaluationContext context;
    context.expression = *expr;
    context.holes = {"dosage", "max_dose"};
    context.meta.domain = "medical";
    context.meta.description = "Ensure dosage doesn't exceed maximum";
    context.meta.confidence_threshold = 0.5;
    
    // LLM suggests max_dose
    auto max_result = PluginManager::instance().try_fill_hole("max_dose", context);
    if (max_result) {
        std::cout << "\nLLM suggested max_dose = " 
                  << std::get_if<Value::Num>(&max_result->value->data)->val << " mg\n";
        
        // Test with different dosages
        Evaluator eval;
        for (double dose : {100.0, 400.0, 600.0}) {
            std::unordered_map<std::string, ValuePtr> values;
            values["max_dose"] = max_result->value;
            values["dosage"] = num(dose);
            
            auto filled = eval.fill_holes(*expr, values);
            if (filled) {
                auto result = eval.eval(*filled);
                if (result && std::get_if<Value::Num>(&(*result)->data)) {
                    std::cout << "Dosage " << dose << " -> adjusted to: " 
                             << std::get_if<Value::Num>(&(*result)->data)->val << "\n";
                }
            }
        }
    }
    
    PluginManager::instance().clear();
}

void list_registered_plugins() {
    std::cout << "\n=== Registered Plugins ===\n";
    
    // Register all plugin types
    PluginManager::instance().register_plugin(std::make_shared<MathContextPlugin>());
    PluginManager::instance().register_plugin(std::make_shared<PatternMatchPlugin>());
    PluginManager::instance().register_plugin(std::make_shared<LLMPlugin>());
    
    auto plugins = PluginManager::instance().list_plugins();
    for (const auto& plugin : plugins) {
        std::cout << "- " << plugin << "\n";
    }
    
    PluginManager::instance().clear();
}

int main() {
    std::cout << "================================================\n";
    std::cout << "   APERTURE PLUGIN SYSTEM DEMO\n";
    std::cout << "   Automatic Hole Inference with LLM\n";
    std::cout << "================================================\n";
    
    demo_math_context_plugin();
    demo_pattern_matching();
    demo_llm_plugin();
    demo_medical_context();
    list_registered_plugins();
    
    std::cout << "\n================================================\n";
    std::cout << "Plugin system demo complete!\n";
    std::cout << "\nKey Features Demonstrated:\n";
    std::cout << "1. Mathematical constant inference (π, e, etc.)\n";
    std::cout << "2. Pattern-based hole filling\n";
    std::cout << "3. LLM context-aware suggestions\n";
    std::cout << "4. Domain-specific knowledge\n";
    std::cout << "5. Confidence scoring and thresholds\n";
    std::cout << "================================================\n";
    
    return 0;
}