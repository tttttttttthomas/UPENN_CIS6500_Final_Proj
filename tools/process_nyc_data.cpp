#include "data/data_loader.h"
#include <iostream>

using namespace flood;

int main(int argc, char* argv[]) {
    std::cout << "NYC Taxi Data Processor" << std::endl;
    std::cout << "=======================" << std::endl;
    
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input_csv> <output_binary>" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Converts NYC taxi CSV data to binary format for faster loading" << std::endl;
        std::cerr << "Expected CSV columns: pickup_longitude, pickup_latitude, ..." << std::endl;
        return 1;
    }
    
    std::string input_file = argv[1];
    std::string output_file = argv[2];
    
    std::cout << "Input: " << input_file << std::endl;
    std::cout << "Output: " << output_file << std::endl;
    std::cout << std::endl;
    
    // Load data
    DataLoader loader;
    std::cout << "Loading CSV data..." << std::endl;
    auto data = loader.loadFromCSV(input_file);
    
    if (data.empty()) {
        std::cerr << "Error: No data loaded from " << input_file << std::endl;
        return 1;
    }
    
    // Clean data
    std::cout << "Cleaning data..." << std::endl;
    auto cleaned = loader.cleanData(data);
    
    // Compute statistics
    auto stats = loader.computeStats(cleaned);
    stats.print();
    std::cout << std::endl;
    
    // Save to binary
    std::cout << "Saving to binary format..." << std::endl;
    loader.saveToBinary(cleaned, output_file);
    
    std::cout << std::endl;
    std::cout << "Done! Processed " << cleaned.size() << " data points" << std::endl;
    
    return 0;
}

