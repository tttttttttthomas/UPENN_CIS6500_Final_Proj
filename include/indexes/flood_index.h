#ifndef FLOOD_INDEX_H
#define FLOOD_INDEX_H

#include "indexes/base_index.h"
#include <map>
#include <functional>

namespace flood {

/**
 * Flood: Learning Multi-dimensional Index
 * Based on "Learning Multi-dimensional Indexes" (SIGMOD '20)
 * 
 * Key features:
 * 1. Cost-model-driven layout optimization
 * 2. Flattening technique for mapping N-D data to 1-D
 * 3. Learned model for predicting query costs
 */
class FloodIndex : public BaseIndex {
public:
    FloodIndex();
    ~FloodIndex() override = default;
    
    void build(const std::vector<DataPoint>& data) override;
    std::vector<DataPoint> query(const QueryRange& range) override;
    double getIndexSize() const override;
    std::string getName() const override { return "Flood"; }
    
    /**
     * Train the cost model using sample queries
     * This should be called after build() but before query()
     */
    void train(const std::vector<QueryRange>& training_queries);

private:
    // Flattened 1D representation of data
    std::vector<DataPoint> flattened_data_;
    
    // Mapping from 1D position to original data point index
    std::vector<size_t> position_map_;
    
    // Data bounds for normalization
    std::vector<double> min_bounds_;
    std::vector<double> max_bounds_;
    size_t dimensions_;
    
    // Cost model parameters (simple linear model)
    struct CostModel {
        double alpha = 1.0;  // Weight for scan cost
        double beta = 0.1;   // Weight for random access cost
        std::vector<double> dimension_weights;  // Importance of each dimension
        
        double predictCost(size_t scan_size, size_t random_accesses) const {
            return alpha * scan_size + beta * random_accesses;
        }
    };
    CostModel cost_model_;
    
    // Flattening function parameters
    // The flattening function maps N-D space to 1-D line
    std::vector<double> projection_vector_;
    
    // Helper functions
    
    /**
     * Compute the 1D flattened key for a data point
     */
    double computeFlattenedKey(const DataPoint& point) const;
    
    /**
     * Learn the optimal projection vector based on data distribution
     */
    void learnProjection(const std::vector<DataPoint>& data);
    
    /**
     * Flatten the multi-dimensional data into 1D
     */
    void flattenData(const std::vector<DataPoint>& data);
    
    /**
     * Train cost model using sample queries
     */
    void trainCostModel(const std::vector<QueryRange>& queries);
    
    /**
     * Map a query range to 1D intervals
     */
    std::vector<std::pair<size_t, size_t>> mapRangeToIntervals(
        const QueryRange& range) const;
    
    /**
     * Binary search to find the start position of a range
     */
    size_t findStartPosition(double key) const;
    
    /**
     * Binary search to find the end position of a range
     */
    size_t findEndPosition(double key) const;
    
    /**
     * Analyze data distribution using simple statistics
     */
    void analyzeDistribution(const std::vector<DataPoint>& data);
};

} // namespace flood

#endif // FLOOD_INDEX_H

