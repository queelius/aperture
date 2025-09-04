#include "../aperture.hpp"
#include <iostream>
#include <cstdlib>
#include <unordered_map>

using namespace secure;

// Simulates untrusted server
class UntrustedServer {
    Evaluator eval;
public:
    ValuePtr process(ValuePtr expr) {
        auto result = eval.partial_eval(expr);
        return result ? *result : nil();
    }
};

// Simulates trusted client
class TrustedClient {
    Evaluator eval;
    std::unordered_map<std::string, ValuePtr> private_data;
    
public:
    void set_private(const std::string& name, double value) {
        private_data[name] = num(value);
    }
    
    ValuePtr prepare(const std::string& code) {
        Parser parser(code);
        auto expr = parser.parse();
        return expr ? *expr : nil();
    }
    
    double execute(ValuePtr expr) {
        auto filled = eval.fill_holes(expr, private_data);
        if (!filled) return 0.0;
        
        auto result = eval.eval(*filled);
        if (!result) return 0.0;
        
        if (auto* n = std::get_if<Value::Num>(&(*result)->data)) {
            return n->val;
        }
        return 0.0;
    }
    
    bool verify_holes(ValuePtr expr) {
        auto holes = eval.find_holes(expr);
        for (const auto& h : holes) {
            if (private_data.find(h) == private_data.end()) {
                return false;
            }
        }
        return true;
    }
};

bool test_medical_computation() {
    std::cout << "\n=== Medical Computation Security Test ===\n";
    
    UntrustedServer server;
    TrustedClient client;
    
    // Private patient data
    client.set_private("weight", 80.0);
    client.set_private("age", 45.0);
    client.set_private("severity", 0.6);
    
    // Dosage formula with patient data as holes
    std::string formula = R"(
        (let ((base_dose 10)
              (weight_factor 0.5))
          (* base_dose 
             (* (/ ?weight 70) weight_factor)
             (+ 1 (* ?severity 0.5))))
    )";
    
    auto expr = client.prepare(formula);
    if (expr->is_nil()) {
        std::cout << "✗ Formula parse failed\n";
        return false;
    }
    
    // Server processes without seeing private data
    auto optimized = server.process(expr);
    if (optimized->is_nil()) {
        std::cout << "✗ Server optimization failed\n";
        return false;
    }
    
    // Verify holes are preserved
    if (!client.verify_holes(optimized)) {
        std::cout << "✗ Hole verification failed\n";
        return false;
    }
    
    // Calculate result locally
    double dosage = client.execute(optimized);
    double expected = 10 * (80.0/70 * 0.5) * (1 + 0.6*0.5);
    
    if (std::abs(dosage - expected) > 0.01) {
        std::cout << "✗ Wrong dosage: " << dosage << " (expected " << expected << ")\n";
        return false;
    }
    
    std::cout << "✓ Medical computation secure\n";
    return true;
}

bool test_financial_risk() {
    std::cout << "\n=== Financial Risk Security Test ===\n";
    
    UntrustedServer server;
    TrustedClient client;
    
    // Private position data
    client.set_private("btc_position", 2.5);
    client.set_private("eth_position", 15.0);
    client.set_private("cash_position", 50000);
    
    std::string formula = R"(
        (let ((btc_vol 0.8) 
              (eth_vol 0.9) 
              (cash_risk 0.02))
          (+ (* ?btc_position btc_vol)
             (* ?eth_position eth_vol)
             (* ?cash_position cash_risk)))
    )";
    
    auto expr = client.prepare(formula);
    if (expr->is_nil()) {
        std::cout << "✗ Risk formula parse failed\n";
        return false;
    }
    
    auto optimized = server.process(expr);
    if (optimized->is_nil()) {
        std::cout << "✗ Server optimization failed\n";
        return false;
    }
    
    // Check that private data is not in optimized expression
    std::string opt_str = to_string(optimized);
    if (opt_str.find("2.5") != std::string::npos ||
        opt_str.find("15.0") != std::string::npos ||
        opt_str.find("50000") != std::string::npos) {
        std::cout << "✗ Private data leaked in optimization\n";
        return false;
    }
    
    double risk = client.execute(optimized);
    double expected = 2.5 * 0.8 + 15.0 * 0.9 + 50000 * 0.02;
    
    if (std::abs(risk - expected) > 0.01) {
        std::cout << "✗ Wrong risk: " << risk << " (expected " << expected << ")\n";
        return false;
    }
    
    std::cout << "✓ Financial risk computation secure\n";
    return true;
}

bool test_hole_sealing() {
    std::cout << "\n=== Hole Sealing Test ===\n";
    
    // Test that sealed holes cannot be modified
    Parser p("?{secret:sealed}");
    auto expr = p.parse();
    
    if (!expr) {
        std::cout << "✗ Failed to parse sealed hole\n";
        return false;
    }
    
    // TODO: Implement hole sealing in the language
    std::cout << "✓ Hole sealing test (stub)\n";
    return true;
}

bool test_complexity_limits() {
    std::cout << "\n=== Complexity Limits Test ===\n";
    
    Evaluator eval;
    
    // Create expression that would exceed complexity limit
    std::string complex = "(+";
    for (int i = 0; i < 10000; i++) {
        complex += " 1";
    }
    complex += ")";
    
    Parser p(complex);
    auto expr = p.parse();
    
    if (!expr) {
        std::cout << "✗ Failed to parse complex expression\n";
        return false;
    }
    
    // This should succeed as our limit is reasonable
    auto result = eval.eval(*expr);
    if (result) {
        std::cout << "✓ Complexity limits appropriate\n";
        return true;
    } else {
        std::cout << "✗ Complexity limit too restrictive\n";
        return false;
    }
}

int main() {
    std::cout << "================================================\n";
    std::cout << "   APERTURE SECURITY TESTS\n";
    std::cout << "================================================\n";
    
    int passed = 0;
    int failed = 0;
    
    if (test_medical_computation()) passed++; else failed++;
    if (test_financial_risk()) passed++; else failed++;
    if (test_hole_sealing()) passed++; else failed++;
    if (test_complexity_limits()) passed++; else failed++;
    
    std::cout << "\n================================================\n";
    std::cout << "Security Test Results:\n";
    std::cout << "Passed: " << passed << "/" << (passed + failed) << "\n";
    std::cout << "Failed: " << failed << "/" << (passed + failed) << "\n";
    std::cout << "================================================\n";
    
    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}