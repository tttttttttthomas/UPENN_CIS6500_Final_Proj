#include "indexes/rtree_index.h"
#include <iostream>
#include <map>

namespace flood {

RTreeIndex::RTreeIndex(size_t max_elements) 
    : max_elements_(max_elements) {}

void RTreeIndex::build(const std::vector<DataPoint>& data) {
    Timer timer;
    
    if (data.empty()) {
        data_size_ = 0;
        build_time_ms_ = timer.elapsed();
        return;
    }
    
    // Keep a copy of the data for later retrieval
    data_copy_ = data;
    
    // Create value pairs (point, id) for R-tree
    std::vector<value_t> values;
    values.reserve(data.size());
    
    for (const auto& dp : data) {
        point_t pt = dataPointToPoint(dp);
        values.push_back(std::make_pair(pt, dp.getId()));
    }
    
    // Build R-tree using packing algorithm (bulk loading)
    rtree_ = std::make_unique<bgi::rtree<value_t, bgi::rstar<16>>>(values);
    
    data_size_ = data.size();
    build_time_ms_ = timer.elapsed();
    
    std::cout << "R*-tree built: " << data.size() << " points, "
              << build_time_ms_ << " ms" << std::endl;
}

std::vector<DataPoint> RTreeIndex::query(const QueryRange& range) {
    std::vector<DataPoint> results;
    
    if (!rtree_ || data_copy_.empty()) {
        return results;
    }
    
    // Convert query range to bounding box
    box_t query_box = queryRangeToBox(range);
    
    // Query R-tree for intersecting points
    std::vector<value_t> rtree_results;
    rtree_->query(bgi::intersects(query_box), std::back_inserter(rtree_results));
    
    // Build a map for fast lookup by ID
    std::map<uint64_t, DataPoint> id_map;
    for (const auto& dp : data_copy_) {
        id_map[dp.getId()] = dp;
    }
    
    // Retrieve actual DataPoints and verify they're in range
    for (const auto& [pt, id] : rtree_results) {
        auto it = id_map.find(id);
        if (it != id_map.end() && range.contains(it->second)) {
            results.push_back(it->second);
        }
    }
    
    return results;
}

double RTreeIndex::getIndexSize() const {
    if (!rtree_) {
        return 0.0;
    }
    
    // Estimate R-tree size
    // Internal nodes + leaf nodes + data copy
    size_t num_points = data_copy_.size();
    size_t nodes_estimate = num_points / max_elements_ * 2; // Rough estimate
    
    // Each internal node contains up to max_elements_ bounding boxes
    size_t node_size = max_elements_ * (sizeof(box_t) + sizeof(void*));
    
    // Data copy size
    size_t data_copy_size = 0;
    if (!data_copy_.empty()) {
        size_t point_size = data_copy_[0].getDimensions() * sizeof(double) + sizeof(uint64_t);
        data_copy_size = num_points * point_size;
    }
    
    size_t total_bytes = nodes_estimate * node_size + data_copy_size;
    return total_bytes / (1024.0 * 1024.0);
}

point_t RTreeIndex::dataPointToPoint(const DataPoint& dp) const {
    // Convert DataPoint to 3D point (pad with 0 if needed)
    return point_t(
        dp.getCoordinate(0), 
        dp.getDimensions() > 1 ? dp.getCoordinate(1) : 0.0,
        dp.getDimensions() > 2 ? dp.getCoordinate(2) : 0.0
    );
}

box_t RTreeIndex::queryRangeToBox(const QueryRange& range) const {
    // Convert QueryRange to bounding box
    point_t min_pt(
        range.getMinBound(0),
        range.getDimensions() > 1 ? range.getMinBound(1) : 0.0,
        range.getDimensions() > 2 ? range.getMinBound(2) : 0.0
    );
    
    point_t max_pt(
        range.getMaxBound(0),
        range.getDimensions() > 1 ? range.getMaxBound(1) : 1.0,
        range.getDimensions() > 2 ? range.getMaxBound(2) : 1.0
    );
    
    return box_t(min_pt, max_pt);
}

} // namespace flood

