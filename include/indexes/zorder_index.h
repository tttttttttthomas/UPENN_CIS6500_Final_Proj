#ifndef ZORDER_INDEX_H
#define ZORDER_INDEX_H

#include "indexes/base_index.h"
#include <map>

namespace flood {

/**
 * Z-order (Morton order) curve implementation
 * Maps multi-dimensional space to 1D using space-filling curve
 */
class ZOrderIndex : public BaseIndex {
public:
    ZOrderIndex();
    ~ZOrderIndex() override = default;
    
    void build(const std::vector<DataPoint>& data) override;
    std::vector<DataPoint> query(const QueryRange& range) override;
    double getIndexSize() const override;
    std::string getName() const override { return "Z-order"; }

private:
    // Sorted map of Z-order key -> DataPoint
    std::map<uint64_t, DataPoint> z_map_;
    size_t dimensions_;
    
    // Normalization bounds for computing Z-order keys
    std::vector<double> min_bounds_;
    std::vector<double> max_bounds_;
    
    // Helper functions
    uint64_t computeZOrder(const DataPoint& point) const;
    uint64_t computeZOrder(const std::vector<double>& coords) const;
    
    // Interleave bits for Z-order encoding
    uint64_t interleaveBits(uint32_t x, uint32_t y) const;
    uint64_t interleaveBits3D(uint32_t x, uint32_t y, uint32_t z) const;
    
    // Normalize coordinate to [0, 2^21 - 1] range for bit interleaving
    uint32_t normalizeCoordinate(double value, size_t dim) const;
    
    // Range decomposition for query
    void getRangeKeys(const QueryRange& range, 
                     std::vector<std::pair<uint64_t, uint64_t>>& key_ranges) const;
};

} // namespace flood

#endif // ZORDER_INDEX_H

