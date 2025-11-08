#include <iostream>

// Simple test framework without external dependencies
// Will be enhanced later

int run_tests();

int main(int argc, char* argv[]) {
    std::cout << "Running Flood Index Tests" << std::endl;
    std::cout << "=========================" << std::endl;
    
    int result = run_tests();
    
    if (result == 0) {
        std::cout << std::endl << "All tests passed!" << std::endl;
    } else {
        std::cout << std::endl << "Some tests failed!" << std::endl;
    }
    
    return result;
}

