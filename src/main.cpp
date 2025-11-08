#include "data/data_point.h"
#include "data/data_loader.h"
#include <iostream>
#include <vector>

using namespace flood;

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "  Flood Learning Index - Test Program  " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    // Test 1: Create some sample data points
    std::cout << "Test 1: Creating sample data points..." << std::endl;
    std::vector<DataPoint> sample_data;
    
    for (int i = 0; i < 10; ++i) {
        std::vector<double> coords = {
            -74.0 + i * 0.01,  // longitude
            40.7 + i * 0.01,   // latitude
            static_cast<double>(i)  // some other dimension
        };
        sample_data.emplace_back(coords, i);
    }
    
    std::cout << "Created " << sample_data.size() << " sample points" << std::endl;
    std::cout << "First point: " << sample_data[0].toString() << std::endl;
    std::cout << "Last point: " << sample_data.back().toString() << std::endl;
    std::cout << std::endl;
    
    // Test 2: Test query range
    std::cout << "Test 2: Testing query range..." << std::endl;
    std::vector<double> min_bounds = {-74.05, 40.70, 0.0};
    std::vector<double> max_bounds = {-73.95, 40.80, 5.0};
    QueryRange range(min_bounds, max_bounds);
    
    std::cout << "Query range: " << range.toString() << std::endl;
    std::cout << "Range volume: " << range.getVolume() << std::endl;
    
    int count = 0;
    for (const auto& point : sample_data) {
        if (range.contains(point)) {
            count++;
        }
    }
    std::cout << "Points within range: " << count << " / " << sample_data.size() << std::endl;
    std::cout << std::endl;
    
    // Test 3: Test DataLoader
    std::cout << "Test 3: Testing DataLoader..." << std::endl;
    DataLoader loader;
    
    // Save sample data to binary file
    std::string binary_file = "/tmp/test_data.bin";
    loader.saveToBinary(sample_data, binary_file);
    
    // Load it back
    auto loaded_data = loader.loadFromBinary(binary_file);
    std::cout << "Loaded " << loaded_data.size() << " points from binary file" << std::endl;
    
    // Compute statistics
    auto stats = loader.computeStats(loaded_data);
    stats.print();
    std::cout << std::endl;
    
    std::cout << "========================================" << std::endl;
    std::cout << "  All tests completed successfully!    " << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}

