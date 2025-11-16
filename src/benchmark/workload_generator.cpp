#include "benchmark/workload_generator.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <limits>

namespace flood {

WorkloadGenerator::WorkloadGenerator(uint32_t seed) : rng_(seed) {}

std::vector<QueryRange> WorkloadGenerator::generateWorkload(
    const std::vector<DataPoint>& data,
    const WorkloadConfig& config) {
    
    rng_.seed(config.seed);
    
    switch (config.type) {
        case WorkloadType::SPATIAL:
            return generateSpatialWorkload(data, config.num_queries, config.selectivity);
        
        case WorkloadType::TEMPORAL:
            return generateTemporalWorkload(data, config.num_queries, 
                                          config.selectivity, config.temporal_range_hours);
        
        case WorkloadType::MIXED:
            return generateMixedWorkload(data, config.num_queries, config.selectivity);
        
        default:
            return {};
    }
}

std::vector<QueryRange> WorkloadGenerator::generateSpatialWorkload(
    const std::vector<DataPoint>& data,
    size_t num_queries,
    double selectivity) {
    
    std::vector<QueryRange> workload;
    workload.reserve(num_queries);
    
    if (data.empty()) {
        return workload;
    }
    
    // Compute data bounds
    std::vector<double> min_bounds, max_bounds;
    computeDataBounds(data, min_bounds, max_bounds);
    
    size_t dimensions = data[0].getDimensions();
    
    // For spatial workload, we only query first 2 dimensions (lon, lat)
    // Other dimensions are unrestricted
    for (size_t i = 0; i < num_queries; ++i) {
        std::vector<double> query_min = min_bounds;
        std::vector<double> query_max = max_bounds;
        
        // Generate random spatial range for first 2 dimensions
        for (size_t dim = 0; dim < std::min(size_t(2), dimensions); ++dim) {
            double domain_size = max_bounds[dim] - min_bounds[dim];
            double range_size = domain_size * std::sqrt(selectivity);
            
            std::uniform_real_distribution<double> dist(
                min_bounds[dim], 
                max_bounds[dim] - range_size
            );
            
            double start = dist(rng_);
            query_min[dim] = start;
            query_max[dim] = start + range_size;
        }
        
        workload.emplace_back(query_min, query_max);
    }
    
    std::cout << "Generated Workload A (Spatial): " << num_queries 
              << " queries, selectivity=" << selectivity << std::endl;
    
    return workload;
}

std::vector<QueryRange> WorkloadGenerator::generateTemporalWorkload(
    const std::vector<DataPoint>& data,
    size_t num_queries,
    double selectivity,
    double time_range_hours) {
    
    std::vector<QueryRange> workload;
    workload.reserve(num_queries);
    
    if (data.empty()) {
        return workload;
    }
    
    // Compute data bounds
    std::vector<double> min_bounds, max_bounds;
    computeDataBounds(data, min_bounds, max_bounds);
    
    size_t dimensions = data[0].getDimensions();
    
    // For temporal workload, query spatial + temporal (first 3 dimensions)
    for (size_t i = 0; i < num_queries; ++i) {
        std::vector<double> query_min = min_bounds;
        std::vector<double> query_max = max_bounds;
        
        // Spatial dimensions (0, 1)
        for (size_t dim = 0; dim < std::min(size_t(2), dimensions); ++dim) {
            double domain_size = max_bounds[dim] - min_bounds[dim];
            double range_size = domain_size * std::sqrt(selectivity * 0.5);
            
            std::uniform_real_distribution<double> dist(
                min_bounds[dim],
                max_bounds[dim] - range_size
            );
            
            double start = dist(rng_);
            query_min[dim] = start;
            query_max[dim] = start + range_size;
        }
        
        // Temporal dimension (2) - if exists
        if (dimensions > 2) {
            double time_domain = max_bounds[2] - min_bounds[2];
            double time_range = std::min(time_range_hours * 3600.0, time_domain);
            
            std::uniform_real_distribution<double> dist(
                min_bounds[2],
                max_bounds[2] - time_range
            );
            
            double start = dist(rng_);
            query_min[2] = start;
            query_max[2] = start + time_range;
        }
        
        workload.emplace_back(query_min, query_max);
    }
    
    std::cout << "Generated Workload B (Temporal): " << num_queries 
              << " queries, selectivity=" << selectivity << std::endl;
    
    return workload;
}

std::vector<QueryRange> WorkloadGenerator::generateMixedWorkload(
    const std::vector<DataPoint>& data,
    size_t num_queries,
    double selectivity) {
    
    std::vector<QueryRange> workload;
    workload.reserve(num_queries);
    
    if (data.empty()) {
        return workload;
    }
    
    // Compute data bounds
    std::vector<double> min_bounds, max_bounds;
    computeDataBounds(data, min_bounds, max_bounds);
    
    size_t dimensions = data[0].getDimensions();
    
    // For mixed workload, query all dimensions
    for (size_t i = 0; i < num_queries; ++i) {
        workload.push_back(
            generateRandomQuery(min_bounds, max_bounds, dimensions, selectivity)
        );
    }
    
    std::cout << "Generated Workload C (Mixed): " << num_queries 
              << " queries, selectivity=" << selectivity << std::endl;
    
    return workload;
}

void WorkloadGenerator::saveWorkload(const std::vector<QueryRange>& workload,
                                    const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filepath << std::endl;
        return;
    }
    
    // Write number of queries and dimensions
    if (!workload.empty()) {
        file << workload.size() << " " << workload[0].getDimensions() << "\n";
        
        for (const auto& query : workload) {
            // Write min bounds
            for (size_t i = 0; i < query.getDimensions(); ++i) {
                file << query.getMinBound(i) << " ";
            }
            // Write max bounds
            for (size_t i = 0; i < query.getDimensions(); ++i) {
                file << query.getMaxBound(i) << " ";
            }
            file << "\n";
        }
    }
    
    file.close();
    std::cout << "Saved " << workload.size() << " queries to " << filepath << std::endl;
}

std::vector<QueryRange> WorkloadGenerator::loadWorkload(const std::string& filepath) {
    std::vector<QueryRange> workload;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for reading: " << filepath << std::endl;
        return workload;
    }
    
    size_t num_queries, dimensions;
    file >> num_queries >> dimensions;
    
    workload.reserve(num_queries);
    
    for (size_t i = 0; i < num_queries; ++i) {
        std::vector<double> min_bounds(dimensions);
        std::vector<double> max_bounds(dimensions);
        
        for (size_t j = 0; j < dimensions; ++j) {
            file >> min_bounds[j];
        }
        for (size_t j = 0; j < dimensions; ++j) {
            file >> max_bounds[j];
        }
        
        workload.emplace_back(min_bounds, max_bounds);
    }
    
    file.close();
    std::cout << "Loaded " << workload.size() << " queries from " << filepath << std::endl;
    
    return workload;
}

QueryRange WorkloadGenerator::generateRandomQuery(
    const std::vector<double>& min_bounds,
    const std::vector<double>& max_bounds,
    size_t dimensions,
    double selectivity) {
    
    std::vector<double> query_min(dimensions);
    std::vector<double> query_max(dimensions);
    
    // For each dimension, generate a range
    double per_dim_selectivity = std::pow(selectivity, 1.0 / dimensions);
    
    for (size_t dim = 0; dim < dimensions; ++dim) {
        double domain_size = max_bounds[dim] - min_bounds[dim];
        double range_size = domain_size * per_dim_selectivity;
        
        std::uniform_real_distribution<double> dist(
            min_bounds[dim],
            max_bounds[dim] - range_size
        );
        
        double start = dist(rng_);
        query_min[dim] = start;
        query_max[dim] = start + range_size;
    }
    
    return QueryRange(query_min, query_max);
}

void WorkloadGenerator::computeDataBounds(
    const std::vector<DataPoint>& data,
    std::vector<double>& min_bounds,
    std::vector<double>& max_bounds) {
    
    if (data.empty()) {
        return;
    }
    
    size_t dimensions = data[0].getDimensions();
    min_bounds.resize(dimensions, std::numeric_limits<double>::max());
    max_bounds.resize(dimensions, std::numeric_limits<double>::lowest());
    
    for (const auto& point : data) {
        for (size_t i = 0; i < dimensions; ++i) {
            double coord = point.getCoordinate(i);
            min_bounds[i] = std::min(min_bounds[i], coord);
            max_bounds[i] = std::max(max_bounds[i], coord);
        }
    }
}

} // namespace flood
