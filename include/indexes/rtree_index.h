#ifndef RTREE_INDEX_H
#define RTREE_INDEX_H

#include "indexes/base_index.h"
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>

namespace flood {

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

// Type definitions for Boost.Geometry R-tree
typedef bg::model::point<double, 3, bg::cs::cartesian> point_t;
typedef bg::model::box<point_t> box_t;
typedef std::pair<point_t, uint64_t> value_t;

/**
 * R*-tree implementation using Boost.Geometry
 * This is the baseline spatial index for comparison
 */
class RTreeIndex : public BaseIndex {
public:
    RTreeIndex(size_t max_elements = 16);
    ~RTreeIndex() override = default;
    
    void build(const std::vector<DataPoint>& data) override;
    std::vector<DataPoint> query(const QueryRange& range) override;
    double getIndexSize() const override;
    std::string getName() const override { return "R*-tree"; }

private:
    // R-tree with R* algorithm, max 16 elements per node
    std::unique_ptr<bgi::rtree<value_t, bgi::rstar<16>>> rtree_;
    std::vector<DataPoint> data_copy_;  // Keep a copy for returning results
    size_t max_elements_;
    
    // Helper functions
    point_t dataPointToPoint(const DataPoint& dp) const;
    box_t queryRangeToBox(const QueryRange& range) const;
};

} // namespace flood

#endif // RTREE_INDEX_H

