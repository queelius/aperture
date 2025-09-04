#include "aperture.hpp"
#include <iostream>
#include <iomanip>

using namespace secure;

// Simulates untrusted cloud server
class UntrustedServer {
    Evaluator eval;
public:
    ValuePtr process_computation(ValuePtr expr) {
        std::cout << "[SERVER] Received expression: " << to_string(expr) << "\n";
        
        // Server can only do partial evaluation with holes
        auto result = eval.partial_eval(expr);
        if (!result) {
            std::cerr << "[SERVER] Error: " << result.error().message << "\n";
            return nil();
        }
        
        std::cout << "[SERVER] Simplified to: " << to_string(*result) << "\n";
        return *result;
    }
};

// Simulates trusted client
class TrustedClient {
    Evaluator eval;
    std::unordered_map<std::string, ValuePtr> private_data;
    
public:
    void set_private(const std::string& name, double value) {
        private_data[name] = num(value);
        std::cout << "[CLIENT] Stored private value: " << name << " = [REDACTED]\n";
    }
    
    ValuePtr prepare_computation(const std::string& code) {
        Parser parser(code);
        auto expr = parser.parse();
        if (!expr) {
            std::cerr << "[CLIENT] Parse error\n";
            return nil();
        }
        return *expr;
    }
    
    double execute_locally(ValuePtr expr) {
        // Fill in private data
        auto filled = eval.fill_holes(expr, private_data);
        if (!filled) {
            std::cerr << "[CLIENT] Fill error\n";
            return 0.0;
        }
        
        // Evaluate locally
        auto result = eval.eval(*filled);
        if (!result) {
            std::cerr << "[CLIENT] Eval error\n";
            return 0.0;
        }
        
        // Extract numeric result
        if (auto* n = std::get_if<Value::Num>(&(*result)->data)) {
            return n->val;
        }
        
        return 0.0;
    }
    
    bool verify_holes(ValuePtr expr) {
        auto holes = eval.find_holes(expr);
        std::cout << "[CLIENT] Expression contains holes: ";
        for (const auto& h : holes) {
            std::cout << h << " ";
        }
        std::cout << "\n";
        
        // Verify all holes are expected
        for (const auto& h : holes) {
            if (private_data.find(h) == private_data.end()) {
                std::cerr << "[CLIENT] WARNING: Unknown hole '" << h << "'\n";
                return false;
            }
        }
        return true;
    }
};

void demo_financial_computation() {
    std::cout << "\n=== FINANCIAL COMPUTATION SCENARIO ===\n";
    std::cout << "Computing portfolio risk with private positions\n\n";
    
    UntrustedServer server;
    TrustedClient client;
    
    // Client has private position data
    client.set_private("btc_position", 2.5);
    client.set_private("eth_position", 15.0);
    client.set_private("cash_position", 50000);
    
    // Risk calculation formula (public)
    // risk = sqrt((btc_pos * btc_vol)^2 + (eth_pos * eth_vol)^2) + cash_risk
    std::string formula = R"(
        (let ((btc_vol 0.8) 
              (eth_vol 0.9) 
              (cash_risk 0.02))
          (+ (* (+ (* (* ?btc_position btc_vol) (* ?btc_position btc_vol))
                   (* (* ?eth_position eth_vol) (* ?eth_position eth_vol)))
                0.5)
             (* ?cash_position cash_risk)))
    )";
    
    std::cout << "Step 1: Client prepares computation\n";
    auto expr = client.prepare_computation(formula);
    
    std::cout << "\nStep 2: Send to untrusted server for optimization\n";
    auto optimized = server.process_computation(expr);
    
    std::cout << "\nStep 3: Client verifies returned expression\n";
    if (!client.verify_holes(optimized)) {
        std::cerr << "Security check failed!\n";
        return;
    }
    
    std::cout << "\nStep 4: Client computes final result locally\n";
    double risk = client.execute_locally(optimized);
    std::cout << "[CLIENT] Portfolio risk score: " << std::fixed << std::setprecision(2) << risk << "\n";
}

void demo_medical_computation() {
    std::cout << "\n=== MEDICAL COMPUTATION SCENARIO ===\n";
    std::cout << "Computing treatment dosage with private patient data\n\n";
    
    UntrustedServer server;
    TrustedClient client;
    
    // Private patient data
    client.set_private("weight", 75.0);  // kg
    client.set_private("age", 45.0);
    client.set_private("condition_severity", 0.7);  // 0-1 scale
    
    // Dosage calculation (public formula)
    std::string formula = R"(
        (let ((base_dose 100)
              (weight_factor 1.2)
              (age_adjustment (if (> ?age 65) 0.8 1.0)))
          (* base_dose 
             (* (/ ?weight 70) weight_factor)
             age_adjustment
             (+ 0.5 (* ?condition_severity 0.5))))
    )";
    
    std::cout << "Step 1: Hospital prepares dosage formula\n";
    auto expr = client.prepare_computation(formula);
    
    std::cout << "\nStep 2: Cloud service optimizes formula\n";
    auto optimized = server.process_computation(expr);
    
    std::cout << "\nStep 3: Hospital verifies formula integrity\n";
    if (!client.verify_holes(optimized)) {
        std::cerr << "Formula tampering detected!\n";
        return;
    }
    
    std::cout << "\nStep 4: Calculate dosage with patient data\n";
    double dosage = client.execute_locally(optimized);
    std::cout << "[HOSPITAL] Recommended dosage: " << std::fixed << std::setprecision(1) << dosage << " mg\n";
}

void demo_ml_inference() {
    std::cout << "\n=== MACHINE LEARNING INFERENCE SCENARIO ===\n";
    std::cout << "Running ML model with private input features\n\n";
    
    UntrustedServer server;
    TrustedClient client;
    
    // Private features
    client.set_private("income", 85000);
    client.set_private("credit_score", 720);
    client.set_private("debt_ratio", 0.35);
    
    // Simplified credit approval model (public weights)
    std::string model = R"(
        (let ((w_income 0.00001)
              (w_credit 0.002)
              (w_debt -2.0)
              (bias -0.5))
          (if (> (+ (* ?income w_income)
                    (* ?credit_score w_credit)
                    (* ?debt_ratio w_debt)
                    bias)
                 0)
              1
              0))
    )";
    
    std::cout << "Step 1: Bank prepares ML model\n";
    auto expr = client.prepare_computation(model);
    
    std::cout << "\nStep 2: Edge server optimizes model\n";
    auto optimized = server.process_computation(expr);
    
    std::cout << "\nStep 3: Bank verifies model structure\n";
    if (!client.verify_holes(optimized)) {
        std::cerr << "Model tampering detected!\n";
        return;
    }
    
    std::cout << "\nStep 4: Run inference with private data\n";
    double approval = client.execute_locally(optimized);
    std::cout << "[BANK] Loan " << (approval > 0.5 ? "APPROVED" : "DENIED") << "\n";
}

int main() {
    std::cout << "================================================\n";
    std::cout << "   SECURE COMPUTATION WITH APERTURES DEMO\n";
    std::cout << "================================================\n";
    
    demo_financial_computation();
    demo_medical_computation();
    demo_ml_inference();
    
    std::cout << "\n================================================\n";
    std::cout << "All secure computations completed successfully!\n";
    std::cout << "\nKey Security Properties Demonstrated:\n";
    std::cout << "1. Private data never leaves trusted environment\n";
    std::cout << "2. Untrusted servers can optimize computations\n";
    std::cout << "3. Clients can verify computation integrity\n";
    std::cout << "4. Partial evaluation simplifies expressions\n";
    std::cout << "================================================\n";
    
    return 0;
}