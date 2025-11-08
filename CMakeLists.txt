cmake_minimum_required(VERSION 3.15)
project(FloodIndex VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# macOS specific settings
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum macOS deployment version")
    execute_process(
        COMMAND xcrun --show-sdk-path
        OUTPUT_VARIABLE OSX_SDK_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(CMAKE_OSX_SYSROOT ${OSX_SDK_PATH})
    
    # Ensure C++ headers are found
    include_directories(SYSTEM "${OSX_SDK_PATH}/usr/include/c++/v1")
    include_directories(SYSTEM "${OSX_SDK_PATH}/usr/include")
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Find required packages
find_package(Boost 1.70 REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    message(STATUS "Boost found: ${Boost_VERSION}")
endif()

# Optional: Find OpenMP for parallel processing
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    message(STATUS "OpenMP found")
endif()

# Source files
set(SOURCES
    src/data/data_point.cpp
    src/data/data_loader.cpp
    src/indexes/base_index.cpp
    # src/indexes/rtree_index.cpp  # TODO: Fix Boost compatibility issue
    src/indexes/kdtree_index.cpp
    src/indexes/zorder_index.cpp
    src/indexes/flood_index.cpp
    src/benchmark/workload_generator.cpp
    src/benchmark/benchmark.cpp
)

# Create library
add_library(flood_lib ${SOURCES})
target_include_directories(flood_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Main executable
add_executable(flood_index src/main.cpp)
target_link_libraries(flood_index flood_lib ${Boost_LIBRARIES})

# Benchmark executable
add_executable(run_benchmark src/benchmark/run_benchmark.cpp)
target_link_libraries(run_benchmark flood_lib ${Boost_LIBRARIES})

# Test executable
add_executable(run_tests tests/test_main.cpp tests/test_indexes.cpp)
target_link_libraries(run_tests flood_lib ${Boost_LIBRARIES})

# Data processing tool
add_executable(process_data tools/process_nyc_data.cpp)
target_link_libraries(process_data flood_lib ${Boost_LIBRARIES})

# Enable testing
enable_testing()
add_test(NAME index_tests COMMAND run_tests)

