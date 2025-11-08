#ifndef KDTREE_INDEX_H
#define KDTREE_INDEX_H

#include "indexes/base_index.h"
#include <memory>

namespace flood {

/**
 * k-d tree implementation for multi-dimensional indexing
 * Simple implementation optimized for range queries
 */
class KDTreeIndex : public BaseIndex {
public:
    KDTreeIndex();
    ~KDTreeIndex() override;
    
    void build(const std::vector<DataPoint>& data) override;
    std::vector<DataPoint> query(const QueryRange& range) override;
    double getIndexSize() const override;
    std::string getName() const override { return "k-d Tree"; }

private:
    struct KDNode {
        DataPoint point;
        std::unique_ptr<KDNode> left;
        std::unique_ptr<KDNode> right;
        size_t split_dim;
        
        KDNode(const DataPoint& p, size_t dim) 
            : point(p), split_dim(dim) {}
    };
    
    std::unique_ptr<KDNode> root_;
    size_t dimensions_;
    size_t num_nodes_;
    
    // Helper functions
    std::unique_ptr<KDNode> buildTree(std::vector<DataPoint>& points, 
                                      size_t depth, 
                                      size_t start, 
                                      size_t end);
    
    void rangeQuery(const KDNode* node, 
                   const QueryRange& range, 
                   std::vector<DataPoint>& results) const;
    
    size_t countNodes(const KDNode* node) const;
};

} // namespace flood

#endif // KDTREE_INDEX_H

