#include "benchmark/benchmark.h"
#include "data/data_loader.h"
#include <iostream>

using namespace flood;

int main(int argc, char* argv[]) {
    std::cout << "Flood Index Benchmark Suite" << std::endl;
    std::cout << "============================" << std::endl;
    
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <data_file> <output_dir>" << std::endl;
        return 1;
    }
    
    std::string data_file = argv[1];
    std::string output_dir = argv[2];
    
    std::cout << "Data file: " << data_file << std::endl;
    std::cout << "Output directory: " << output_dir << std::endl;
    std::cout << std::endl;
    
    // TODO: Implement full benchmark suite in Phase 4
    std::cout << "Benchmark framework will be implemented in Phase 4" << std::endl;
    
    return 0;
}

