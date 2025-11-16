#include "benchmark/benchmark.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <numeric>

namespace flood {

Benchmark::Benchmark() : warmup_queries_(0), verbose_(true) {}

std::vector<BenchmarkResult> Benchmark::runSuite(
    const std::vector<std::shared_ptr<BaseIndex>>& indexes,
    const std::vector<DataPoint>& data,
    const std::vector<std::pair<std::string, std::vector<QueryRange>>>& workloads) {
    
    std::vector<BenchmarkResult> all_results;
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "  Running Benchmark Suite" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Data size: " << data.size() << " points" << std::endl;
    std::cout << "Indexes: " << indexes.size() << std::endl;
    std::cout << "Workloads: " << workloads.size() << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    for (const auto& [workload_name, queries] : workloads) {
        std::cout << "\n--- Workload: " << workload_name << " ---" << std::endl;
        std::cout << "Queries: " << queries.size() << std::endl;
        
        for (auto& index : indexes) {
            auto result = runBenchmark(index.get(), data, queries, workload_name);
            all_results.push_back(result);
        }
    }
    
    return all_results;
}

BenchmarkResult Benchmark::runBenchmark(
    BaseIndex* index,
    const std::vector<DataPoint>& data,
    const std::vector<QueryRange>& queries,
    const std::string& workload_name) {
    
    BenchmarkResult result;
    result.index_name = index->getName();
    result.workload_name = workload_name;
    result.total_queries = queries.size();
    
    if (verbose_) {
        std::cout << "\nTesting " << index->getName() << "..." << std::endl;
    }
    
    // Build index
    auto build_start = std::chrono::high_resolution_clock::now();
    index->build(data);
    auto build_end = std::chrono::high_resolution_clock::now();
    
    result.build_time_ms = std::chrono::duration<double, std::milli>(
        build_end - build_start).count();
    result.index_size_mb = index->getIndexSize();
    
    if (verbose_) {
        std::cout << "  Build time: " << result.build_time_ms << " ms" << std::endl;
        std::cout << "  Index size: " << result.index_size_mb << " MB" << std::endl;
    }
    
    // Warmup queries
    if (warmup_queries_ > 0 && !queries.empty()) {
        for (size_t i = 0; i < std::min(warmup_queries_, queries.size()); ++i) {
            index->query(queries[i]);
        }
    }
    
    // Run queries and collect timing
    std::vector<double> query_times;
    std::vector<size_t> result_counts;
    query_times.reserve(queries.size());
    result_counts.reserve(queries.size());
    
    result.total_results = 0;
    
    for (const auto& query : queries) {
        auto query_start = std::chrono::high_resolution_clock::now();
        auto query_results = index->query(query);
        auto query_end = std::chrono::high_resolution_clock::now();
        
        double query_time = std::chrono::duration<double, std::milli>(
            query_end - query_start).count();
        
        query_times.push_back(query_time);
        result_counts.push_back(query_results.size());
        result.total_results += query_results.size();
    }
    
    // Calculate statistics
    result.avg_query_time_ms = std::accumulate(
        query_times.begin(), query_times.end(), 0.0) / query_times.size();
    
    result.median_query_time_ms = calculateMedian(query_times);
    result.p95_query_time_ms = calculatePercentile(query_times, 0.95);
    result.p99_query_time_ms = calculatePercentile(query_times, 0.99);
    
    // Scan overhead (simplified: assume we scan some fraction of data)
    // For a more accurate measure, we'd need to instrument the index
    result.scan_overhead = 1.0;  // Placeholder
    
    if (verbose_) {
        std::cout << "  Avg query time: " << result.avg_query_time_ms << " ms" << std::endl;
        std::cout << "  Median: " << result.median_query_time_ms << " ms" << std::endl;
        std::cout << "  P95: " << result.p95_query_time_ms << " ms" << std::endl;
        std::cout << "  P99: " << result.p99_query_time_ms << " ms" << std::endl;
        std::cout << "  Total results: " << result.total_results << std::endl;
    }
    
    return result;
}

void Benchmark::saveResults(const std::vector<BenchmarkResult>& results,
                           const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filepath << std::endl;
        return;
    }
    
    // Write CSV header
    file << "Index,Workload,BuildTime_ms,IndexSize_MB,AvgQueryTime_ms,"
         << "MedianQueryTime_ms,P95QueryTime_ms,P99QueryTime_ms,"
         << "TotalQueries,TotalResults\n";
    
    // Write results
    for (const auto& result : results) {
        file << result.toCSV() << "\n";
    }
    
    file.close();
    std::cout << "\nResults saved to " << filepath << std::endl;
}

double Benchmark::calculateScanOverhead(
    const std::vector<size_t>& scanned_counts,
    const std::vector<size_t>& result_counts) const {
    
    if (scanned_counts.size() != result_counts.size() || result_counts.empty()) {
        return 1.0;
    }
    
    double total_scanned = std::accumulate(
        scanned_counts.begin(), scanned_counts.end(), 0.0);
    double total_results = std::accumulate(
        result_counts.begin(), result_counts.end(), 0.0);
    
    return total_results > 0 ? total_scanned / total_results : 1.0;
}

double Benchmark::calculateMedian(std::vector<double>& values) const {
    if (values.empty()) {
        return 0.0;
    }
    
    std::sort(values.begin(), values.end());
    size_t n = values.size();
    
    if (n % 2 == 0) {
        return (values[n/2 - 1] + values[n/2]) / 2.0;
    } else {
        return values[n/2];
    }
}

double Benchmark::calculatePercentile(std::vector<double>& values, 
                                      double percentile) const {
    if (values.empty()) {
        return 0.0;
    }
    
    std::sort(values.begin(), values.end());
    size_t index = static_cast<size_t>(values.size() * percentile);
    index = std::min(index, values.size() - 1);
    
    return values[index];
}

std::string BenchmarkResult::toCSV() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(4);
    oss << index_name << ","
        << workload_name << ","
        << build_time_ms << ","
        << index_size_mb << ","
        << avg_query_time_ms << ","
        << median_query_time_ms << ","
        << p95_query_time_ms << ","
        << p99_query_time_ms << ","
        << total_queries << ","
        << total_results;
    return oss.str();
}

void BenchmarkResult::print() const {
    std::cout << "\n=== Benchmark Result ===" << std::endl;
    std::cout << "Index: " << index_name << std::endl;
    std::cout << "Workload: " << workload_name << std::endl;
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Build time: " << build_time_ms << " ms" << std::endl;
    std::cout << "Index size: " << index_size_mb << " MB" << std::endl;
    std::cout << "Avg query time: " << avg_query_time_ms << " ms" << std::endl;
    std::cout << "Median query time: " << median_query_time_ms << " ms" << std::endl;
    std::cout << "P95 query time: " << p95_query_time_ms << " ms" << std::endl;
    std::cout << "P99 query time: " << p99_query_time_ms << " ms" << std::endl;
    std::cout << "Total queries: " << total_queries << std::endl;
    std::cout << "Total results: " << total_results << std::endl;
}

} // namespace flood
