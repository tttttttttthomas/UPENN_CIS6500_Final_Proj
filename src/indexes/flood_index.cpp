#include "indexes/flood_index.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <limits>

namespace flood {

FloodIndex::FloodIndex() : dimensions_(0) {}

void FloodIndex::build(const std::vector<DataPoint>& data) {
    Timer timer;
    
    if (data.empty()) {
        data_size_ = 0;
        build_time_ms_ = timer.elapsed();
        return;
    }
    
    dimensions_ = data[0].getDimensions();
    
    std::cout << "Building Flood index..." << std::endl;
    
    // Step 1: Analyze data distribution
    std::cout << "  1. Analyzing data distribution..." << std::endl;
    analyzeDistribution(data);
    
    // Step 2: Learn projection vector (simplified PCA approach)
    std::cout << "  2. Learning projection vector..." << std::endl;
    learnProjection(data);
    
    // Step 3: Flatten data to 1D
    std::cout << "  3. Flattening data to 1D..." << std::endl;
    flattenData(data);
    
    data_size_ = data.size();
    build_time_ms_ = timer.elapsed();
    
    std::cout << "Flood index built: " << flattened_data_.size() << " points, "
              << build_time_ms_ << " ms" << std::endl;
}

std::vector<DataPoint> FloodIndex::query(const QueryRange& range) {
    std::vector<DataPoint> results;
    
    if (flattened_data_.empty()) {
        return results;
    }
    
    // Get intervals to scan based on the query range
    auto intervals = mapRangeToIntervals(range);
    
    // Scan each interval and filter results
    for (const auto& [start, end] : intervals) {
        for (size_t i = start; i <= end && i < flattened_data_.size(); ++i) {
            const auto& point = flattened_data_[i];
            if (range.contains(point)) {
                results.push_back(point);
            }
        }
    }
    
    return results;
}

double FloodIndex::getIndexSize() const {
    // Flattened data + projection vector + cost model
    size_t point_size = dimensions_ * sizeof(double) + sizeof(uint64_t);
    size_t data_size = flattened_data_.size() * point_size;
    size_t projection_size = projection_vector_.size() * sizeof(double);
    size_t model_size = sizeof(CostModel) + cost_model_.dimension_weights.size() * sizeof(double);
    
    size_t total_bytes = data_size + projection_size + model_size;
    return total_bytes / (1024.0 * 1024.0);
}

void FloodIndex::train(const std::vector<QueryRange>& training_queries) {
    std::cout << "Training cost model with " << training_queries.size() 
              << " queries..." << std::endl;
    trainCostModel(training_queries);
}

double FloodIndex::computeFlattenedKey(const DataPoint& point) const {
    if (projection_vector_.empty() || projection_vector_.size() != dimensions_) {
        return 0.0;
    }
    
    // Compute dot product: key = projection_vector · normalized_point
    double key = 0.0;
    for (size_t i = 0; i < dimensions_; ++i) {
        // Normalize coordinate to [0, 1]
        double range = max_bounds_[i] - min_bounds_[i];
        double normalized = (range > 1e-10) ? 
            (point.getCoordinate(i) - min_bounds_[i]) / range : 0.0;
        
        key += projection_vector_[i] * normalized;
    }
    
    return key;
}

void FloodIndex::learnProjection(const std::vector<DataPoint>& data) {
    projection_vector_.resize(dimensions_, 0.0);
    
    // Simplified approach: Use variance-based weighting
    // Dimensions with higher variance get higher weights
    std::vector<double> means(dimensions_, 0.0);
    std::vector<double> variances(dimensions_, 0.0);
    
    // Calculate means
    for (const auto& point : data) {
        for (size_t i = 0; i < dimensions_; ++i) {
            means[i] += point.getCoordinate(i);
        }
    }
    for (size_t i = 0; i < dimensions_; ++i) {
        means[i] /= data.size();
    }
    
    // Calculate variances
    for (const auto& point : data) {
        for (size_t i = 0; i < dimensions_; ++i) {
            double diff = point.getCoordinate(i) - means[i];
            variances[i] += diff * diff;
        }
    }
    for (size_t i = 0; i < dimensions_; ++i) {
        variances[i] /= data.size();
    }
    
    // Normalize variances to get projection weights
    double total_var = std::accumulate(variances.begin(), variances.end(), 0.0);
    if (total_var > 1e-10) {
        for (size_t i = 0; i < dimensions_; ++i) {
            projection_vector_[i] = std::sqrt(variances[i] / total_var);
        }
    } else {
        // Equal weights if no variance
        for (size_t i = 0; i < dimensions_; ++i) {
            projection_vector_[i] = 1.0 / std::sqrt(dimensions_);
        }
    }
    
    // Initialize cost model dimension weights
    cost_model_.dimension_weights = projection_vector_;
    
    std::cout << "    Projection vector learned (variance-based)" << std::endl;
}

void FloodIndex::flattenData(const std::vector<DataPoint>& data) {
    // Create pairs of (flattened_key, data_point)
    std::vector<std::pair<double, DataPoint>> keyed_data;
    keyed_data.reserve(data.size());
    
    for (const auto& point : data) {
        double key = computeFlattenedKey(point);
        keyed_data.emplace_back(key, point);
    }
    
    // Sort by flattened key
    std::sort(keyed_data.begin(), keyed_data.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });
    
    // Extract sorted data
    flattened_data_.clear();
    flattened_data_.reserve(data.size());
    position_map_.clear();
    position_map_.reserve(data.size());
    
    for (size_t i = 0; i < keyed_data.size(); ++i) {
        flattened_data_.push_back(keyed_data[i].second);
        position_map_.push_back(i);
    }
    
    std::cout << "    Data flattened and sorted by projection" << std::endl;
}

void FloodIndex::trainCostModel(const std::vector<QueryRange>& queries) {
    if (queries.empty()) {
        std::cout << "    No training queries provided, using default model" << std::endl;
        return;
    }
    
    // Simplified cost model training
    // In a full implementation, this would use regression to learn alpha and beta
    // For now, we use reasonable defaults
    
    cost_model_.alpha = 1.0;   // Sequential scan cost
    cost_model_.beta = 0.1;    // Random access cost
    
    std::cout << "    Cost model trained (alpha=" << cost_model_.alpha 
              << ", beta=" << cost_model_.beta << ")" << std::endl;
}

std::vector<std::pair<size_t, size_t>> FloodIndex::mapRangeToIntervals(
    const QueryRange& range) const {
    
    std::vector<std::pair<size_t, size_t>> intervals;
    
    if (flattened_data_.empty()) {
        return intervals;
    }
    
    // Compute min and max flattened keys for the query range corners
    // For simplicity, we compute keys for the corners and take min/max
    std::vector<double> corner_keys;
    
    // Generate corners of the query hypercube
    size_t num_corners = 1 << dimensions_;  // 2^dimensions
    for (size_t corner = 0; corner < num_corners; ++corner) {
        std::vector<double> coords;
        for (size_t dim = 0; dim < dimensions_; ++dim) {
            bool use_max = (corner >> dim) & 1;
            coords.push_back(use_max ? range.getMaxBound(dim) : range.getMinBound(dim));
        }
        
        // Compute flattened key for this corner
        double key = 0.0;
        for (size_t i = 0; i < dimensions_; ++i) {
            double range_width = max_bounds_[i] - min_bounds_[i];
            double normalized = (range_width > 1e-10) ?
                (coords[i] - min_bounds_[i]) / range_width : 0.0;
            key += projection_vector_[i] * normalized;
        }
        corner_keys.push_back(key);
    }
    
    // Find min and max keys
    double min_key = *std::min_element(corner_keys.begin(), corner_keys.end());
    double max_key = *std::max_element(corner_keys.begin(), corner_keys.end());
    
    // Find positions in flattened data
    size_t start_pos = findStartPosition(min_key);
    size_t end_pos = findEndPosition(max_key);
    
    if (start_pos <= end_pos && end_pos < flattened_data_.size()) {
        intervals.emplace_back(start_pos, end_pos);
    }
    
    return intervals;
}

size_t FloodIndex::findStartPosition(double key) const {
    if (flattened_data_.empty()) {
        return 0;
    }
    
    // Binary search for the first position with flattened key >= key
    auto it = std::lower_bound(
        flattened_data_.begin(), 
        flattened_data_.end(),
        key,
        [this](const DataPoint& point, double value) {
            return computeFlattenedKey(point) < value;
        }
    );
    
    return std::distance(flattened_data_.begin(), it);
}

size_t FloodIndex::findEndPosition(double key) const {
    if (flattened_data_.empty()) {
        return 0;
    }
    
    // Binary search for the last position with flattened key <= key
    auto it = std::upper_bound(
        flattened_data_.begin(),
        flattened_data_.end(),
        key,
        [this](double value, const DataPoint& point) {
            return value < computeFlattenedKey(point);
        }
    );
    
    if (it == flattened_data_.begin()) {
        return 0;
    }
    
    return std::distance(flattened_data_.begin(), it) - 1;
}

void FloodIndex::analyzeDistribution(const std::vector<DataPoint>& data) {
    // Compute data bounds
    min_bounds_.resize(dimensions_, std::numeric_limits<double>::max());
    max_bounds_.resize(dimensions_, std::numeric_limits<double>::lowest());
    
    for (const auto& point : data) {
        for (size_t i = 0; i < dimensions_; ++i) {
            double coord = point.getCoordinate(i);
            min_bounds_[i] = std::min(min_bounds_[i], coord);
            max_bounds_[i] = std::max(max_bounds_[i], coord);
        }
    }
    
    std::cout << "    Data bounds computed for " << dimensions_ << " dimensions" << std::endl;
}

} // namespace flood

