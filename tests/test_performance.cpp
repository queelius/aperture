#include "../aperture.hpp"
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <vector>
#include <numeric>
#include <functional>
#include <algorithm>

using namespace secure;
using namespace std::chrono;

struct BenchmarkResult {
    std::string name;
    double mean_us;
    double min_us;
    double max_us;
    int iterations;
};

BenchmarkResult benchmark(const std::string& name, const std::function<void()>& fn, int iterations = 10000) {
    std::vector<double> timings;
    timings.reserve(iterations);
    
    // Warmup
    for (int i = 0; i < 100; i++) {
        fn();
    }
    
    // Actual benchmark
    for (int i = 0; i < iterations; i++) {
        auto start = high_resolution_clock::now();
        fn();
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<nanoseconds>(end - start).count() / 1000.0;
        timings.push_back(duration);
    }
    
    double sum = std::accumulate(timings.begin(), timings.end(), 0.0);
    double mean = sum / timings.size();
    double min = *std::min_element(timings.begin(), timings.end());
    double max = *std::max_element(timings.begin(), timings.end());
    
    return {name, mean, min, max, iterations};
}

void print_result(const BenchmarkResult& r) {
    std::cout << r.name << ":\n";
    std::cout << "  Mean: " << r.mean_us << " µs\n";
    std::cout << "  Min:  " << r.min_us << " µs\n";
    std::cout << "  Max:  " << r.max_us << " µs\n";
    std::cout << "  Iterations: " << r.iterations << "\n";
}

int main() {
    std::cout << "================================================\n";
    std::cout << "   APERTURE PERFORMANCE BENCHMARKS\n";
    std::cout << "================================================\n\n";
    
    Evaluator eval;
    bool all_passed = true;
    
    // Benchmark 1: Simple arithmetic
    {
        Parser p("(+ (* 2 3) (* 4 5))");
        auto expr = p.parse();
        if (expr) {
            auto result = benchmark("Simple arithmetic", [&]() {
                eval.eval(*expr);
            });
            print_result(result);
            if (result.mean_us > 10) {
                std::cout << "  ⚠ Performance warning: slower than expected\n";
                all_passed = false;
            } else {
                std::cout << "  ✓ Performance acceptable\n";
            }
        }
    }
    
    std::cout << "\n";
    
    // Benchmark 2: Let bindings
    {
        Parser p("(let ((x 10) (y 20)) (+ (* x 2) (* y 3)))");
        auto expr = p.parse();
        if (expr) {
            auto result = benchmark("Let bindings", [&]() {
                eval.eval(*expr);
            });
            print_result(result);
            if (result.mean_us > 20) {
                std::cout << "  ⚠ Performance warning: slower than expected\n";
                all_passed = false;
            } else {
                std::cout << "  ✓ Performance acceptable\n";
            }
        }
    }
    
    std::cout << "\n";
    
    // Benchmark 3: Lambda application
    {
        Parser p("((lambda (x y) (+ x y)) 10 20)");
        auto expr = p.parse();
        if (expr) {
            auto result = benchmark("Lambda application", [&]() {
                eval.eval(*expr);
            });
            print_result(result);
            if (result.mean_us > 25) {
                std::cout << "  ⚠ Performance warning: slower than expected\n";
                all_passed = false;
            } else {
                std::cout << "  ✓ Performance acceptable\n";
            }
        }
    }
    
    std::cout << "\n";
    
    // Benchmark 4: Partial evaluation with holes
    {
        Parser p("(+ (* ?x 2) (* 3 4) (- 10 5))");
        auto expr = p.parse();
        if (expr) {
            auto result = benchmark("Partial eval with holes", [&]() {
                eval.partial_eval(*expr);
            });
            print_result(result);
            if (result.mean_us > 30) {
                std::cout << "  ⚠ Performance warning: slower than expected\n";
                all_passed = false;
            } else {
                std::cout << "  ✓ Performance acceptable\n";
            }
        }
    }
    
    std::cout << "\n";
    
    // Benchmark 5: Deep nesting (stress test)
    {
        std::string deep = "(+";
        for (int i = 0; i < 50; i++) {
            deep += " (+ " + std::to_string(i) + " 1)";
        }
        deep += ")";
        
        Parser p(deep);
        auto expr = p.parse();
        if (expr) {
            auto result = benchmark("Deep nesting (50 levels)", [&]() {
                eval.eval(*expr);
            }, 1000);  // Fewer iterations for stress test
            print_result(result);
            if (result.mean_us > 100) {
                std::cout << "  ⚠ Performance warning: slower than expected\n";
                all_passed = false;
            } else {
                std::cout << "  ✓ Performance acceptable\n";
            }
        }
    }
    
    std::cout << "\n";
    
    // Benchmark 6: Hole filling
    {
        Parser p("(+ ?x ?y ?z)");
        auto expr = p.parse();
        if (expr) {
            auto partial = eval.partial_eval(*expr);
            if (partial) {
                std::unordered_map<std::string, ValuePtr> values{
                    {"x", num(10)}, {"y", num(20)}, {"z", num(30)}
                };
                
                auto result = benchmark("Hole filling", [&]() {
                    eval.fill_holes(*partial, values);
                });
                print_result(result);
                if (result.mean_us > 15) {
                    std::cout << "  ⚠ Performance warning: slower than expected\n";
                    all_passed = false;
                } else {
                    std::cout << "  ✓ Performance acceptable\n";
                }
            }
        }
    }
    
    std::cout << "\n================================================\n";
    if (all_passed) {
        std::cout << "✓ All performance benchmarks passed\n";
    } else {
        std::cout << "⚠ Some benchmarks showed suboptimal performance\n";
    }
    std::cout << "================================================\n";
    
    return all_passed ? EXIT_SUCCESS : EXIT_FAILURE;
}