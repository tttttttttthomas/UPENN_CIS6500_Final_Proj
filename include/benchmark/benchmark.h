#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "indexes/base_index.h"
#include "benchmark/workload_generator.h"
#include <string>
#include <vector>
#include <memory>

namespace flood {

/**
 * Results from running a benchmark
 */
struct BenchmarkResult {
    std::string index_name;
    std::string workload_name;
    
    double build_time_ms;
    double index_size_mb;
    double avg_query_time_ms;
    double median_query_time_ms;
    double p95_query_time_ms;
    double p99_query_time_ms;
    double scan_overhead;
    
    size_t total_queries;
    size_t total_results;
    
    std::string toCSV() const;
    void print() const;
};

/**
 * Benchmark: Runs performance evaluation on indexes
 */
class Benchmark {
public:
    Benchmark();
    
    /**
     * Run a complete benchmark suite
     * Tests all indexes against all workloads
     */
    std::vector<BenchmarkResult> runSuite(
        const std::vector<std::shared_ptr<BaseIndex>>& indexes,
        const std::vector<DataPoint>& data,
        const std::vector<std::pair<std::string, std::vector<QueryRange>>>& workloads);
    
    /**
     * Run benchmark for a single index and workload
     */
    BenchmarkResult runBenchmark(
        BaseIndex* index,
        const std::vector<DataPoint>& data,
        const std::vector<QueryRange>& queries,
        const std::string& workload_name);
    
    /**
     * Save benchmark results to CSV file
     */
    void saveResults(const std::vector<BenchmarkResult>& results,
                    const std::string& filepath);
    
    /**
     * Set warmup queries (run before actual benchmark)
     */
    void setWarmupQueries(size_t num) { warmup_queries_ = num; }
    
    /**
     * Enable/disable verbose output
     */
    void setVerbose(bool v) { verbose_ = v; }

private:
    size_t warmup_queries_;
    bool verbose_;
    
    // Helper functions
    double calculateScanOverhead(
        const std::vector<size_t>& scanned_counts,
        const std::vector<size_t>& result_counts) const;
    
    double calculateMedian(std::vector<double>& values) const;
    double calculatePercentile(std::vector<double>& values, double percentile) const;
};

} // namespace flood

#endif // BENCHMARK_H

