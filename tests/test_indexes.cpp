#include "data/data_point.h"
#include "data/data_loader.h"
#include <iostream>
#include <cassert>

using namespace flood;

void test_data_point() {
    std::cout << "Testing DataPoint... ";
    
    std::vector<double> coords = {1.0, 2.0, 3.0};
    DataPoint p1(coords, 0);
    
    assert(p1.getDimensions() == 3);
    assert(p1.getCoordinate(0) == 1.0);
    assert(p1.getId() == 0);
    
    std::cout << "PASSED" << std::endl;
}

void test_query_range() {
    std::cout << "Testing QueryRange... ";
    
    std::vector<double> min_b = {0.0, 0.0};
    std::vector<double> max_b = {1.0, 1.0};
    QueryRange range(min_b, max_b);
    
    DataPoint p1({0.5, 0.5}, 0);
    DataPoint p2({1.5, 1.5}, 1);
    
    assert(range.contains(p1));
    assert(!range.contains(p2));
    
    std::cout << "PASSED" << std::endl;
}

void test_data_loader() {
    std::cout << "Testing DataLoader... ";
    
    DataLoader loader;
    std::vector<DataPoint> data;
    
    for (int i = 0; i < 5; ++i) {
        data.emplace_back(std::vector<double>{static_cast<double>(i), 
                                              static_cast<double>(i*2)}, i);
    }
    
    // Test binary save/load
    std::string temp_file = "/tmp/test_loader.bin";
    loader.saveToBinary(data, temp_file);
    auto loaded = loader.loadFromBinary(temp_file);
    
    assert(loaded.size() == data.size());
    assert(loaded[0].getId() == data[0].getId());
    
    std::cout << "PASSED" << std::endl;
}

int run_tests() {
    try {
        test_data_point();
        test_query_range();
        test_data_loader();
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}

