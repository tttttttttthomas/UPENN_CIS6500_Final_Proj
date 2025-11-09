#include "indexes/zorder_index.h"
#include <iostream>
#include <algorithm>
#include <limits>

namespace flood {

ZOrderIndex::ZOrderIndex() : dimensions_(0) {}

void ZOrderIndex::build(const std::vector<DataPoint>& data) {
    Timer timer;
    
    if (data.empty()) {
        data_size_ = 0;
        build_time_ms_ = timer.elapsed();
        return;
    }
    
    dimensions_ = data[0].getDimensions();
    
    // Compute min/max bounds for normalization
    min_bounds_.resize(dimensions_, std::numeric_limits<double>::max());
    max_bounds_.resize(dimensions_, std::numeric_limits<double>::lowest());
    
    for (const auto& point : data) {
        for (size_t i = 0; i < dimensions_; ++i) {
            double coord = point.getCoordinate(i);
            min_bounds_[i] = std::min(min_bounds_[i], coord);
            max_bounds_[i] = std::max(max_bounds_[i], coord);
        }
    }
    
    // Build Z-order map
    z_map_.clear();
    for (const auto& point : data) {
        uint64_t z_key = computeZOrder(point);
        z_map_[z_key] = point;
    }
    
    data_size_ = data.size();
    build_time_ms_ = timer.elapsed();
    
    std::cout << "Z-order index built: " << z_map_.size() << " points, "
              << build_time_ms_ << " ms" << std::endl;
}

std::vector<DataPoint> ZOrderIndex::query(const QueryRange& range) {
    std::vector<DataPoint> results;
    
    if (z_map_.empty()) {
        return results;
    }
    
    // Compute Z-order key ranges for the query
    std::vector<std::pair<uint64_t, uint64_t>> key_ranges;
    getRangeKeys(range, key_ranges);
    
    // Scan all points in the Z-order map and check if they're in range
    // For a simple implementation, we scan all and filter
    // A more advanced implementation would use range decomposition
    for (const auto& [z_key, point] : z_map_) {
        if (range.contains(point)) {
            results.push_back(point);
        }
    }
    
    return results;
}

double ZOrderIndex::getIndexSize() const {
    // Map contains: key (uint64_t) + DataPoint
    size_t point_size = dimensions_ * sizeof(double) + sizeof(uint64_t);
    size_t entry_size = sizeof(uint64_t) + point_size + sizeof(void*) * 3; // RB-tree overhead
    size_t total_bytes = z_map_.size() * entry_size;
    
    return total_bytes / (1024.0 * 1024.0);
}

uint64_t ZOrderIndex::computeZOrder(const DataPoint& point) const {
    std::vector<double> coords;
    for (size_t i = 0; i < dimensions_; ++i) {
        coords.push_back(point.getCoordinate(i));
    }
    return computeZOrder(coords);
}

uint64_t ZOrderIndex::computeZOrder(const std::vector<double>& coords) const {
    if (coords.size() < 2) return 0;
    
    // Normalize coordinates to integer range
    uint32_t x = normalizeCoordinate(coords[0], 0);
    uint32_t y = normalizeCoordinate(coords[1], 1);
    
    if (coords.size() == 2) {
        return interleaveBits(x, y);
    } else {
        uint32_t z = normalizeCoordinate(coords[2], 2);
        return interleaveBits3D(x, y, z);
    }
}

uint64_t ZOrderIndex::interleaveBits(uint32_t x, uint32_t y) const {
    // Interleave bits of x and y to create Morton code
    // We use 21 bits per dimension to fit in 64 bits (21*2 = 42 bits)
    
    uint64_t result = 0;
    for (int i = 0; i < 21; ++i) {
        result |= ((uint64_t)(x & (1u << i)) << i) | ((uint64_t)(y & (1u << i)) << (i + 1));
    }
    return result;
}

uint64_t ZOrderIndex::interleaveBits3D(uint32_t x, uint32_t y, uint32_t z) const {
    // Interleave bits of x, y, z for 3D Morton code
    // We use 21 bits per dimension to fit in 64 bits (21*3 = 63 bits)
    
    uint64_t result = 0;
    for (int i = 0; i < 21; ++i) {
        result |= ((uint64_t)(x & (1u << i)) << (i * 2)) |
                  ((uint64_t)(y & (1u << i)) << (i * 2 + 1)) |
                  ((uint64_t)(z & (1u << i)) << (i * 2 + 2));
    }
    return result;
}

uint32_t ZOrderIndex::normalizeCoordinate(double value, size_t dim) const {
    if (dim >= dimensions_) return 0;
    
    // Normalize to [0, 1] range
    double range = max_bounds_[dim] - min_bounds_[dim];
    if (range < 1e-10) return 0;  // Avoid division by zero
    
    double normalized = (value - min_bounds_[dim]) / range;
    normalized = std::max(0.0, std::min(1.0, normalized));
    
    // Scale to [0, 2^21 - 1] range (21 bits)
    const uint32_t MAX_VAL = (1u << 21) - 1;
    return static_cast<uint32_t>(normalized * MAX_VAL);
}

void ZOrderIndex::getRangeKeys(const QueryRange& range, 
                              std::vector<std::pair<uint64_t, uint64_t>>& key_ranges) const {
    // Simplified implementation: compute min and max Z-order keys
    // A full implementation would decompose the range into multiple intervals
    
    std::vector<double> min_coords, max_coords;
    for (size_t i = 0; i < dimensions_; ++i) {
        min_coords.push_back(range.getMinBound(i));
        max_coords.push_back(range.getMaxBound(i));
    }
    
    uint64_t min_key = computeZOrder(min_coords);
    uint64_t max_key = computeZOrder(max_coords);
    
    key_ranges.push_back({min_key, max_key});
}

} // namespace flood

