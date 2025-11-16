#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include "data/data_point.h"
#include "indexes/kdtree_index.h"
#include "indexes/zorder_index.h"
#include "indexes/rtree_index.h"
#include "indexes/flood_index.h"

using namespace flood;

void test_correctness() {
    std::cout << "=== Testing Index Correctness ===" << std::endl;
    std::cout << std::endl;
    
    // Create test data: 10x10 grid
    std::vector<DataPoint> data;
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
            std::vector<double> coords = {(double)x, (double)y};
            data.emplace_back(coords, x * 10 + y);
        }
    }
    
    // Test query: [2, 5] x [3, 7]
    // Expected: points in (2,3), (2,4), ..., (5,7) = 4x5 = 20 points
    std::vector<double> min_bounds = {2.0, 3.0};
    std::vector<double> max_bounds = {5.0, 7.0};
    QueryRange query(min_bounds, max_bounds);
    
    std::cout << "Test data: 10x10 grid (100 points)" << std::endl;
    std::cout << "Query: [2, 5] x [3, 7]" << std::endl;
    std::cout << "Expected: 20 points" << std::endl;
    std::cout << std::endl;
    
    // Test k-d tree
    {
        KDTreeIndex kdtree;
        kdtree.build(data);
        auto results = kdtree.query(query);
        
        std::cout << "k-d Tree: " << results.size() << " points";
        if (results.size() == 20) {
            std::cout << " ✓" << std::endl;
        } else {
            std::cout << " ✗ (expected 20)" << std::endl;
        }
    }
    
    // Test Z-order
    {
        ZOrderIndex zorder;
        zorder.build(data);
        auto results = zorder.query(query);
        
        std::cout << "Z-order: " << results.size() << " points";
        if (results.size() == 20) {
            std::cout << " ✓" << std::endl;
        } else {
            std::cout << " ✗ (expected 20)" << std::endl;
        }
    }
    
    // Test R*-tree
    {
        RTreeIndex rtree;
        rtree.build(data);
        auto results = rtree.query(query);
        
        std::cout << "R*-tree: " << results.size() << " points";
        if (results.size() == 20) {
            std::cout << " ✓" << std::endl;
        } else {
            std::cout << " ✗ (expected 20)" << std::endl;
        }
    }
    
    // Test Flood
    {
        FloodIndex flood;
        flood.build(data);
        auto results = flood.query(query);
        
        std::cout << "Flood:   " << results.size() << " points";
        if (results.size() == 20) {
            std::cout << " ✓" << std::endl;
        } else {
            std::cout << " ✗ (expected 20)" << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void test_performance() {
    std::cout << "=== Testing Index Performance ===" << std::endl;
    std::cout << std::endl;
    
    // Create larger dataset
    std::vector<DataPoint> data;
    for (int i = 0; i < 10000; ++i) {
        std::vector<double> coords = {
            (double)(i % 100),
            (double)(i / 100)
        };
        data.emplace_back(coords, i);
    }
    
    std::vector<double> min_bounds = {25.0, 25.0};
    std::vector<double> max_bounds = {75.0, 75.0};
    QueryRange query(min_bounds, max_bounds);
    
    std::cout << "Test data: 10,000 points" << std::endl;
    std::cout << "Query: [25, 75] x [25, 75]" << std::endl;
    std::cout << std::endl;
    
    std::cout << std::fixed;
    std::cout.precision(3);
    
    // Test k-d tree
    {
        KDTreeIndex kdtree;
        kdtree.build(data);
        auto results = kdtree.query(query);
        
        std::cout << "k-d Tree:" << std::endl;
        std::cout << "  Build time: " << kdtree.getBuildTime() << " ms" << std::endl;
        std::cout << "  Index size: " << kdtree.getIndexSize() << " MB" << std::endl;
        std::cout << "  Query results: " << results.size() << " points" << std::endl;
        std::cout << std::endl;
    }
    
    // Test Z-order
    {
        ZOrderIndex zorder;
        zorder.build(data);
        auto results = zorder.query(query);
        
        std::cout << "Z-order:" << std::endl;
        std::cout << "  Build time: " << zorder.getBuildTime() << " ms" << std::endl;
        std::cout << "  Index size: " << zorder.getIndexSize() << " MB" << std::endl;
        std::cout << "  Query results: " << results.size() << " points" << std::endl;
        std::cout << std::endl;
    }
    
    // Test R*-tree
    {
        RTreeIndex rtree;
        rtree.build(data);
        auto results = rtree.query(query);
        
        std::cout << "R*-tree:" << std::endl;
        std::cout << "  Build time: " << rtree.getBuildTime() << " ms" << std::endl;
        std::cout << "  Index size: " << rtree.getIndexSize() << " MB" << std::endl;
        std::cout << "  Query results: " << results.size() << " points" << std::endl;
        std::cout << std::endl;
    }
    
    // Test Flood
    {
        FloodIndex flood;
        flood.build(data);
        auto results = flood.query(query);
        
        std::cout << "Flood:" << std::endl;
        std::cout << "  Build time: " << flood.getBuildTime() << " ms" << std::endl;
        std::cout << "  Index size: " << flood.getIndexSize() << " MB" << std::endl;
        std::cout << "  Query results: " << results.size() << " points" << std::endl;
        std::cout << std::endl;
    }
}

int main() {
    test_correctness();
    test_performance();
    
    std::cout << "=== All tests completed! ===" << std::endl;
    return 0;
}

