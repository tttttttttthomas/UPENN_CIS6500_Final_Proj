#ifndef WORKLOAD_GENERATOR_H
#define WORKLOAD_GENERATOR_H

#include "data/data_point.h"
#include <vector>
#include <random>
#include <string>

namespace flood {

/**
 * Workload types as defined in the proposal
 */
enum class WorkloadType {
    SPATIAL,      // Workload A: Pure spatial queries (lon, lat)
    TEMPORAL,     // Workload B: Spatio-temporal queries (lon, lat, time)
    MIXED         // Workload C: Mixed queries with all dimensions
};

/**
 * Configuration for workload generation
 */
struct WorkloadConfig {
    WorkloadType type;
    size_t num_queries;
    double selectivity;  // Expected fraction of data returned (0.0 - 1.0)
    
    // For spatial workloads
    double spatial_range_ratio;  // Range size as fraction of domain
    
    // For temporal workloads
    double temporal_range_hours;  // Time range in hours
    
    // Random seed
    uint32_t seed = 42;
    
    WorkloadConfig(WorkloadType t, size_t n, double sel) 
        : type(t), num_queries(n), selectivity(sel), 
          spatial_range_ratio(0.01), temporal_range_hours(24.0) {}
};

/**
 * WorkloadGenerator: Creates synthetic query workloads for benchmarking
 */
class WorkloadGenerator {
public:
    WorkloadGenerator(uint32_t seed = 42);
    
    /**
     * Generate a workload based on the data distribution
     */
    std::vector<QueryRange> generateWorkload(
        const std::vector<DataPoint>& data,
        const WorkloadConfig& config);
    
    /**
     * Generate Workload A: Pure spatial queries
     */
    std::vector<QueryRange> generateSpatialWorkload(
        const std::vector<DataPoint>& data,
        size_t num_queries,
        double selectivity);
    
    /**
     * Generate Workload B: Spatio-temporal queries
     */
    std::vector<QueryRange> generateTemporalWorkload(
        const std::vector<DataPoint>& data,
        size_t num_queries,
        double selectivity,
        double time_range_hours);
    
    /**
     * Generate Workload C: Mixed queries (all dimensions)
     */
    std::vector<QueryRange> generateMixedWorkload(
        const std::vector<DataPoint>& data,
        size_t num_queries,
        double selectivity);
    
    /**
     * Save workload to file for reproducibility
     */
    void saveWorkload(const std::vector<QueryRange>& workload, 
                     const std::string& filepath);
    
    /**
     * Load workload from file
     */
    std::vector<QueryRange> loadWorkload(const std::string& filepath);

private:
    std::mt19937 rng_;
    
    // Helper functions
    QueryRange generateRandomQuery(
        const std::vector<double>& min_bounds,
        const std::vector<double>& max_bounds,
        size_t dimensions,
        double selectivity);
    
    void computeDataBounds(
        const std::vector<DataPoint>& data,
        std::vector<double>& min_bounds,
        std::vector<double>& max_bounds);
};

} // namespace flood

#endif // WORKLOAD_GENERATOR_H

