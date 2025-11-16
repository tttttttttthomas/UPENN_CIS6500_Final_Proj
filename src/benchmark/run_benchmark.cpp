#include <iostream>
#include <memory>
#include <vector>
#include <string>

#include "data/data_point.h"
#include "indexes/kdtree_index.h"
#include "indexes/zorder_index.h"
#include "indexes/rtree_index.h"
#include "indexes/flood_index.h"
#include "benchmark/workload_generator.h"
#include "benchmark/benchmark.h"

using namespace flood;

// Generate synthetic data for testing
std::vector<DataPoint> generateSyntheticData(size_t num_points, size_t dimensions) {
    std::vector<DataPoint> data;
    data.reserve(num_points);
    
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(0.0, 100.0);
    
    for (size_t i = 0; i < num_points; ++i) {
        std::vector<double> coords(dimensions);
        for (size_t j = 0; j < dimensions; ++j) {
            coords[j] = dist(rng);
        }
        data.emplace_back(coords, i);
    }
    
    return data;
}

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "  Flood Index Benchmark Suite" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // Configuration
    size_t data_size = 50000;  // 50K points
    size_t dimensions = 3;      // 3D data (x, y, time)
    size_t num_queries = 100;   // 100 queries per workload
    
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Data size: " << data_size << " points" << std::endl;
    std::cout << "  Dimensions: " << dimensions << std::endl;
    std::cout << "  Queries per workload: " << num_queries << std::endl;
    std::cout << std::endl;
    
    // Generate synthetic data
    std::cout << "Generating synthetic data..." << std::endl;
    auto data = generateSyntheticData(data_size, dimensions);
    std::cout << "Generated " << data.size() << " points" << std::endl;
    std::cout << std::endl;
    
    // Create indexes
    std::cout << "Creating indexes..." << std::endl;
    std::vector<std::shared_ptr<BaseIndex>> indexes;
    indexes.push_back(std::make_shared<KDTreeIndex>());
    indexes.push_back(std::make_shared<ZOrderIndex>());
    indexes.push_back(std::make_shared<RTreeIndex>());
    indexes.push_back(std::make_shared<FloodIndex>());
    std::cout << "Created " << indexes.size() << " indexes" << std::endl;
    std::cout << std::endl;
    
    // Generate workloads
    std::cout << "Generating workloads..." << std::endl;
    WorkloadGenerator generator(42);
    
    std::vector<std::pair<std::string, std::vector<QueryRange>>> workloads;
    
    // Workload A: Spatial queries (0.1% selectivity)
    WorkloadConfig config_a(WorkloadType::SPATIAL, num_queries, 0.001);
    auto workload_a = generator.generateWorkload(data, config_a);
    workloads.push_back({"Workload_A_Spatial", workload_a});
    
    // Workload B: Temporal queries (0.5% selectivity)
    WorkloadConfig config_b(WorkloadType::TEMPORAL, num_queries, 0.005);
    config_b.temporal_range_hours = 24.0;
    auto workload_b = generator.generateWorkload(data, config_b);
    workloads.push_back({"Workload_B_Temporal", workload_b});
    
    // Workload C: Mixed queries (1% selectivity)
    WorkloadConfig config_c(WorkloadType::MIXED, num_queries, 0.01);
    auto workload_c = generator.generateWorkload(data, config_c);
    workloads.push_back({"Workload_C_Mixed", workload_c});
    
    std::cout << "Generated " << workloads.size() << " workloads" << std::endl;
    std::cout << std::endl;
    
    // Run benchmarks
    Benchmark benchmark;
    benchmark.setVerbose(true);
    benchmark.setWarmupQueries(10);
    
    auto results = benchmark.runSuite(indexes, data, workloads);
    
    // Save results
    std::string output_file = "benchmark_results.csv";
    benchmark.saveResults(results, output_file);
    
    // Print summary
    std::cout << "\n========================================" << std::endl;
    std::cout << "  Benchmark Summary" << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::cout << std::fixed << std::setprecision(4);
    
    for (const auto& [workload_name, _] : workloads) {
        std::cout << "\n" << workload_name << ":" << std::endl;
        std::cout << std::setw(12) << "Index"
                  << std::setw(15) << "Build(ms)"
                  << std::setw(15) << "Size(MB)"
                  << std::setw(15) << "AvgQuery(ms)"
                  << std::setw(15) << "P95(ms)" << std::endl;
        std::cout << std::string(71, '-') << std::endl;
        
        for (const auto& result : results) {
            if (result.workload_name == workload_name) {
                std::cout << std::setw(12) << result.index_name
                          << std::setw(15) << result.build_time_ms
                          << std::setw(15) << result.index_size_mb
                          << std::setw(15) << result.avg_query_time_ms
                          << std::setw(15) << result.p95_query_time_ms << std::endl;
            }
        }
    }
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "Benchmark completed successfully!" << std::endl;
    std::cout << "Results saved to: " << output_file << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}
