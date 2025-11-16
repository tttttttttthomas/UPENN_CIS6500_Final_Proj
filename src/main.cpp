#include <iostream>
#include <vector>
#include <memory>
#include "data/data_point.h"
#include "indexes/kdtree_index.h"
#include "indexes/zorder_index.h"
#include "indexes/rtree_index.h"
#include "indexes/flood_index.h"

using namespace flood;

int main() {
    std::cout << "=== Flood Index Project - Full Index Test ===" << std::endl;
    std::cout << std::endl;
    
    // Create sample 2D data points
    std::vector<DataPoint> data;
    std::cout << "Creating sample data (1000 points in 2D)..." << std::endl;
    
    for (size_t i = 0; i < 1000; ++i) {
        std::vector<double> coords = {
            (double)(i % 100),           // x: 0-99
            (double)(i / 100)            // y: 0-9
        };
        data.emplace_back(coords, i);
    }
    
    std::cout << "Sample data created: " << data.size() << " points" << std::endl;
    std::cout << std::endl;
    
    // Define a query range: [10, 20] x [2, 5]
    std::vector<double> min_bounds = {10.0, 2.0};
    std::vector<double> max_bounds = {20.0, 5.0};
    QueryRange query(min_bounds, max_bounds);
    
    std::cout << "Query range: [" << min_bounds[0] << ", " << max_bounds[0] 
              << "] x [" << min_bounds[1] << ", " << max_bounds[1] << "]" << std::endl;
    std::cout << std::endl;
    
    // Test k-d Tree
    {
        std::cout << "--- Testing k-d Tree ---" << std::endl;
        KDTreeIndex kdtree;
        kdtree.build(data);
        
        auto results = kdtree.query(query);
        std::cout << "Query results: " << results.size() << " points" << std::endl;
        std::cout << "Index size: " << kdtree.getIndexSize() << " MB" << std::endl;
        std::cout << "Build time: " << kdtree.getBuildTime() << " ms" << std::endl;
        std::cout << std::endl;
    }
    
    // Test Z-order
    {
        std::cout << "--- Testing Z-order Index ---" << std::endl;
        ZOrderIndex zorder;
        zorder.build(data);
        
        auto results = zorder.query(query);
        std::cout << "Query results: " << results.size() << " points" << std::endl;
        std::cout << "Index size: " << zorder.getIndexSize() << " MB" << std::endl;
        std::cout << "Build time: " << zorder.getBuildTime() << " ms" << std::endl;
        std::cout << std::endl;
    }
    
    // Test R*-tree
    {
        std::cout << "--- Testing R*-tree Index ---" << std::endl;
        RTreeIndex rtree;
        rtree.build(data);
        
        auto results = rtree.query(query);
        std::cout << "Query results: " << results.size() << " points" << std::endl;
        std::cout << "Index size: " << rtree.getIndexSize() << " MB" << std::endl;
        std::cout << "Build time: " << rtree.getBuildTime() << " ms" << std::endl;
        std::cout << std::endl;
    }
    
    // Test Flood (Learning Index)
    {
        std::cout << "--- Testing Flood Index (Learning) ---" << std::endl;
        FloodIndex flood;
        flood.build(data);
        
        auto results = flood.query(query);
        std::cout << "Query results: " << results.size() << " points" << std::endl;
        std::cout << "Index size: " << flood.getIndexSize() << " MB" << std::endl;
        std::cout << "Build time: " << flood.getBuildTime() << " ms" << std::endl;
        std::cout << std::endl;
    }
    
    std::cout << "=== All tests completed successfully! ===" << std::endl;
    
    return 0;
}
