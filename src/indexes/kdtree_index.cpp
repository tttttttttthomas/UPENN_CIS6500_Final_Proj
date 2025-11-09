#include "indexes/kdtree_index.h"
#include <iostream>
#include <algorithm>

namespace flood {

KDTreeIndex::KDTreeIndex() : dimensions_(0), num_nodes_(0) {}

KDTreeIndex::~KDTreeIndex() = default;

void KDTreeIndex::build(const std::vector<DataPoint>& data) {
    Timer timer;
    
    if (data.empty()) {
        data_size_ = 0;
        build_time_ms_ = timer.elapsed();
        return;
    }
    
    // Get dimensions from first point
    dimensions_ = data[0].getDimensions();
    
    // Copy data for sorting
    std::vector<DataPoint> points = data;
    
    // Build the tree recursively
    root_ = buildTree(points, 0, 0, points.size());
    
    // Count nodes
    num_nodes_ = countNodes(root_.get());
    
    data_size_ = data.size();
    build_time_ms_ = timer.elapsed();
    
    std::cout << "k-d Tree built: " << num_nodes_ << " nodes, " 
              << build_time_ms_ << " ms" << std::endl;
}

std::vector<DataPoint> KDTreeIndex::query(const QueryRange& range) {
    std::vector<DataPoint> results;
    
    if (!root_) {
        return results;
    }
    
    // Perform recursive range query
    rangeQuery(root_.get(), range, results);
    
    return results;
}

double KDTreeIndex::getIndexSize() const {
    // Each node contains:
    // - DataPoint (dimensions_ * sizeof(double) + overhead)
    // - Two pointers (2 * sizeof(void*))
    // - split_dim (sizeof(size_t))
    
    size_t point_size = dimensions_ * sizeof(double) + sizeof(uint64_t);
    size_t node_overhead = 2 * sizeof(void*) + sizeof(size_t);
    size_t total_bytes = num_nodes_ * (point_size + node_overhead);
    
    return total_bytes / (1024.0 * 1024.0); // Convert to MB
}

std::unique_ptr<KDTreeIndex::KDNode> KDTreeIndex::buildTree(
    std::vector<DataPoint>& points, size_t depth, size_t start, size_t end) {
    
    if (start >= end) {
        return nullptr;
    }
    
    // Choose splitting dimension (cycle through dimensions)
    size_t split_dim = depth % dimensions_;
    
    // Find median by sorting along split dimension
    size_t mid = start + (end - start) / 2;
    
    // Partition around median using nth_element
    std::nth_element(
        points.begin() + start,
        points.begin() + mid,
        points.begin() + end,
        [split_dim](const DataPoint& a, const DataPoint& b) {
            return a.getCoordinate(split_dim) < b.getCoordinate(split_dim);
        }
    );
    
    // Create node with median point
    auto node = std::make_unique<KDNode>(points[mid], split_dim);
    
    // Recursively build left and right subtrees
    node->left = buildTree(points, depth + 1, start, mid);
    node->right = buildTree(points, depth + 1, mid + 1, end);
    
    return node;
}

void KDTreeIndex::rangeQuery(const KDNode* node, const QueryRange& range, 
                            std::vector<DataPoint>& results) const {
    if (!node) {
        return;
    }
    
    // Check if current point is in range
    if (range.contains(node->point)) {
        results.push_back(node->point);
    }
    
    // Get split dimension and value
    size_t dim = node->split_dim;
    double split_value = node->point.getCoordinate(dim);
    double range_min = range.getMinBound(dim);
    double range_max = range.getMaxBound(dim);
    
    // Recursively search left subtree if range overlaps
    if (range_min <= split_value) {
        rangeQuery(node->left.get(), range, results);
    }
    
    // Recursively search right subtree if range overlaps
    if (range_max >= split_value) {
        rangeQuery(node->right.get(), range, results);
    }
}

size_t KDTreeIndex::countNodes(const KDNode* node) const {
    if (!node) {
        return 0;
    }
    return 1 + countNodes(node->left.get()) + countNodes(node->right.get());
}

} // namespace flood

