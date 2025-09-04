#pragma once
#include "plugin_interface.hpp"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <curl/curl.h>  // For HTTP requests
#include <nlohmann/json.hpp>  // JSON parsing

namespace aperture::plugins {

// Configuration management for LLM providers
class LLMConfig {
public:
    struct Provider {
        std::string name = "openai";
        std::string api_endpoint = "https://api.openai.com/v1/chat/completions";
        std::string api_key = "";
        std::string model = "gpt-4";
        double temperature = 0.3;
        int max_tokens = 150;
        
        // Optional headers for specific providers
        std::unordered_map<std::string, std::string> headers;
    };
    
    Provider provider;
    double min_confidence = 0.7;
    bool verbose = false;
    bool cache_responses = true;
    std::string cache_dir = "~/.aperture/cache";
    
    // Load configuration from ~/.aperture/config.json
    static LLMConfig load() {
        LLMConfig config;
        
        std::string home = std::getenv("HOME") ? std::getenv("HOME") : "";
        if (home.empty()) return config;
        
        std::string config_path = home + "/.aperture/config.json";
        std::ifstream file(config_path);
        
        if (!file.is_open()) {
            // Try to create default config
            create_default_config(config_path);
            return config;
        }
        
        try {
            nlohmann::json j;
            file >> j;
            
            if (j.contains("llm")) {
                auto& llm = j["llm"];
                
                if (llm.contains("provider")) {
                    config.provider.name = llm["provider"];
                }
                
                // Provider-specific endpoints
                if (config.provider.name == "ollama") {
                    config.provider.api_endpoint = llm.value("endpoint", "http://localhost:11434/v1/chat/completions");
                } else if (config.provider.name == "openai") {
                    config.provider.api_endpoint = llm.value("endpoint", "https://api.openai.com/v1/chat/completions");
                } else if (config.provider.name == "anthropic") {
                    // Anthropic has different API format, would need adapter
                    config.provider.api_endpoint = llm.value("endpoint", "https://api.anthropic.com/v1/messages");
                } else {
                    // Custom provider
                    config.provider.api_endpoint = llm.value("endpoint", "");
                }
                
                config.provider.model = llm.value("model", "gpt-4");
                config.provider.api_key = llm.value("api_key", "");
                config.provider.temperature = llm.value("temperature", 0.3);
                config.provider.max_tokens = llm.value("max_tokens", 150);
                
                // Check environment variable for API key
                if (config.provider.api_key.empty()) {
                    const char* env_key = nullptr;
                    if (config.provider.name == "openai") {
                        env_key = std::getenv("OPENAI_API_KEY");
                    } else if (config.provider.name == "anthropic") {
                        env_key = std::getenv("ANTHROPIC_API_KEY");
                    }
                    if (env_key) {
                        config.provider.api_key = env_key;
                    }
                }
            }
            
            if (j.contains("aperture")) {
                auto& ap = j["aperture"];
                config.min_confidence = ap.value("min_confidence", 0.7);
                config.verbose = ap.value("verbose", false);
                config.cache_responses = ap.value("cache", true);
            }
            
        } catch (const std::exception& e) {
            std::cerr << "[LLM] Error loading config: " << e.what() << "\n";
        }
        
        return config;
    }
    
private:
    static void create_default_config(const std::string& path) {
        // Create ~/.aperture directory if it doesn't exist
        std::string dir = path.substr(0, path.find_last_of('/'));
        std::string mkdir_cmd = "mkdir -p " + dir;
        std::system(mkdir_cmd.c_str());
        
        // Write default configuration
        nlohmann::json config = {
            {"llm", {
                {"provider", "openai"},
                {"model", "gpt-4"},
                {"temperature", 0.3},
                {"max_tokens", 150},
                {"api_key", ""},  // User should set this
                {"_comment", "Set api_key or use environment variable OPENAI_API_KEY"}
            }},
            {"aperture", {
                {"min_confidence", 0.7},
                {"verbose", false},
                {"cache", true}
            }},
            {"providers", {
                {"ollama", {
                    {"endpoint", "http://localhost:11434/v1/chat/completions"},
                    {"models", {"llama2", "codellama", "mistral"}}
                }},
                {"openai", {
                    {"endpoint", "https://api.openai.com/v1/chat/completions"},
                    {"models", {"gpt-4", "gpt-3.5-turbo"}}
                }},
                {"custom", {
                    {"endpoint", "http://your-server/v1/chat/completions"},
                    {"api_key_header", "Authorization"}
                }}
            }}
        };
        
        std::ofstream file(path);
        if (file.is_open()) {
            file << config.dump(2);
            file.close();
            std::cout << "[LLM] Created default config at " << path << "\n";
            std::cout << "[LLM] Please set your API key in the config file\n";
        }
    }
};

// OpenAI-compatible API client (works with multiple providers)
class OpenAIAPIClient {
    LLMConfig::Provider config;
    
    static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }
    
public:
    explicit OpenAIAPIClient(const LLMConfig::Provider& cfg) : config(cfg) {}
    
    std::optional<std::string> complete(const std::string& prompt) {
        CURL* curl = curl_easy_init();
        if (!curl) return std::nullopt;
        
        // Build request body (OpenAI format)
        nlohmann::json request = {
            {"model", config.model},
            {"messages", {
                {{"role", "system"}, {"content", "You are a program analysis assistant. Respond only with JSON."}},
                {{"role", "user"}, {"content", prompt}}
            }},
            {"temperature", config.temperature},
            {"max_tokens", config.max_tokens},
            {"response_format", {{"type", "json_object"}}}  // For providers that support it
        };
        
        std::string request_body = request.dump();
        std::string response_body;
        
        // Setup CURL
        curl_easy_setopt(curl, CURLOPT_URL, config.api_endpoint.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_body.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);
        
        // Headers
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        
        // API key handling (different for different providers)
        if (!config.api_key.empty()) {
            if (config.name == "openai") {
                std::string auth = "Authorization: Bearer " + config.api_key;
                headers = curl_slist_append(headers, auth.c_str());
            } else if (config.name == "anthropic") {
                std::string auth = "x-api-key: " + config.api_key;
                headers = curl_slist_append(headers, auth.c_str());
            } else {
                // Generic authorization header
                std::string auth = "Authorization: Bearer " + config.api_key;
                headers = curl_slist_append(headers, auth.c_str());
            }
        }
        
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        
        // Perform request
        CURLcode res = curl_easy_perform(curl);
        
        // Cleanup
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        
        if (res != CURLE_OK) {
            return std::nullopt;
        }
        
        // Extract content from response
        try {
            auto response_json = nlohmann::json::parse(response_body);
            
            // Handle different response formats
            if (response_json.contains("choices") && !response_json["choices"].empty()) {
                // OpenAI format
                return response_json["choices"][0]["message"]["content"];
            } else if (response_json.contains("content")) {
                // Direct content format
                return response_json["content"];
            } else if (response_json.contains("response")) {
                // Alternative format
                return response_json["response"];
            }
        } catch (const std::exception& e) {
            std::cerr << "[LLM] Error parsing response: " << e.what() << "\n";
        }
        
        return std::nullopt;
    }
};

// Main LLM plugin using provider abstraction
class LLMPlugin : public Plugin {
    LLMConfig config;
    std::unique_ptr<OpenAIAPIClient> client;
    mutable std::unordered_map<std::string, HoleFillResult> cache;
    
public:
    explicit LLMPlugin(const LLMConfig& cfg = LLMConfig::load()) 
        : config(cfg) {
        client = std::make_unique<OpenAIAPIClient>(config.provider);
        
        if (config.verbose) {
            std::cout << "[LLM] Using provider: " << config.provider.name << "\n";
            std::cout << "[LLM] Model: " << config.provider.model << "\n";
            std::cout << "[LLM] Endpoint: " << config.provider.api_endpoint << "\n";
        }
    }
    
    std::string name() const override { 
        return "LLM_" + config.provider.name; 
    }
    
    std::string description() const override { 
        return "LLM-based hole inference using " + config.provider.name; 
    }
    
    std::string version() const override { 
        return "2.0.0"; 
    }
    
    std::optional<HoleFillResult> suggest_hole_value(
        const std::string& hole_name,
        const EvaluationContext& context
    ) override {
        
        // Check cache
        if (config.cache_responses) {
            auto it = cache.find(hole_name);
            if (it != cache.end()) {
                if (config.verbose) {
                    std::cout << "[LLM] Using cached value for " << hole_name << "\n";
                }
                return it->second;
            }
        }
        
        // Build prompt
        auto prompt = build_structured_prompt(hole_name, context);
        
        if (config.verbose) {
            std::cout << "[LLM] Querying for hole: " << hole_name << "\n";
        }
        
        // Query LLM
        auto response = client->complete(prompt);
        if (!response) {
            if (config.verbose) {
                std::cerr << "[LLM] No response from API\n";
            }
            return std::nullopt;
        }
        
        // Parse response
        auto result = parse_json_response(hole_name, *response);
        
        if (result && result->confidence >= config.min_confidence) {
            // Cache result
            if (config.cache_responses) {
                cache[hole_name] = *result;
            }
            return result;
        }
        
        return std::nullopt;
    }
    
private:
    std::string build_structured_prompt(
        const std::string& hole_name,
        const EvaluationContext& context
    ) const {
        std::stringstream prompt;
        
        prompt << "Analyze this partial program and infer the value for the hole.\n\n";
        prompt << "Program: " << context.get_full_context() << "\n";
        prompt << "Hole name: " << hole_name << "\n";
        
        if (!context.meta.domain.empty()) {
            prompt << "Domain: " << context.meta.domain << "\n";
        }
        
        // Analyze context
        std::string expr = context.get_full_context();
        prompt << "\nContext clues:\n";
        
        if (expr.find("(+ ?" + hole_name) != std::string::npos) {
            prompt << "- Used in addition (consider 0 for identity)\n";
        }
        if (expr.find("(* ?" + hole_name) != std::string::npos) {
            prompt << "- Used in multiplication (consider 1 for identity)\n";
        }
        if (hole_name.find("rate") != std::string::npos) {
            prompt << "- Name suggests a rate (typically 0.0-1.0)\n";
        }
        if (hole_name.find("count") != std::string::npos) {
            prompt << "- Name suggests an integer\n";
        }
        
        prompt << "\nRespond with JSON only:\n";
        prompt << "{\n";
        prompt << "  \"value\": <number or string>,\n";
        prompt << "  \"type\": \"number\" or \"symbol\" or \"string\",\n";
        prompt << "  \"confidence\": 0.0 to 1.0,\n";
        prompt << "  \"reasoning\": \"brief explanation\"\n";
        prompt << "}\n";
        
        return prompt.str();
    }
    
    std::optional<HoleFillResult> parse_json_response(
        const std::string& hole_name,
        const std::string& json_str
    ) const {
        try {
            auto j = nlohmann::json::parse(json_str);
            
            secure::ValuePtr value;
            std::string type = j.value("type", "number");
            
            if (type == "number") {
                value = secure::num(j.value("value", 0.0));
            } else if (type == "symbol") {
                value = secure::sym(j.value("value", std::string("")));
            } else if (type == "string") {
                value = secure::str(j.value("value", std::string("")));
            } else {
                return std::nullopt;
            }
            
            return HoleFillResult{
                hole_name,
                value,
                j.value("confidence", 0.0),
                j.value("reasoning", ""),
                "llm_" + config.provider.name
            };
            
        } catch (const std::exception& e) {
            if (config.verbose) {
                std::cerr << "[LLM] JSON parse error: " << e.what() << "\n";
            }
            return std::nullopt;
        }
    }
};

} // namespace aperture::plugins