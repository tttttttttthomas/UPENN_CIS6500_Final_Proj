# Flood Learning Index - Multi-dimensional Indexing

This project implements and evaluates **Flood**, a learned multi-dimensional index based on the paper "Learning Multi-dimensional Indexes" (SIGMOD '20), and compares it against traditional spatial indexing methods.

## Project Overview

### Goal
Implement and benchmark Flood learning index against baseline spatial indexes (R*-tree, k-d tree, Z-order) on real-world NYC taxi data.

### Indexes Implemented
1. **Flood Index** - Learning-based multi-dimensional index (main contribution)
2. **R*-tree** - Traditional spatial index (using Boost.Geometry)
3. **k-d Tree** - Binary space partitioning tree
4. **Z-order (Morton)** - Space-filling curve based index

### Workloads
- **Workload A**: Pure spatial queries (longitude, latitude)
- **Workload B**: Spatio-temporal queries (longitude, latitude, time)
- **Workload C**: Mixed queries (all dimensions)

## Project Structure

```
.
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
├── include/                # Header files
│   ├── data/
│   │   ├── data_point.h       # Data point and query range classes
│   │   └── data_loader.h      # Data loading and preprocessing
│   ├── indexes/
│   │   ├── base_index.h       # Abstract base class for all indexes
│   │   ├── flood_index.h      # Flood learning index (TO BE IMPLEMENTED)
│   │   ├── rtree_index.h      # R*-tree implementation
│   │   ├── kdtree_index.h     # k-d tree implementation
│   │   └── zorder_index.h     # Z-order curve implementation
│   └── benchmark/
│       ├── workload_generator.h  # Query workload generation
│       └── benchmark.h           # Benchmarking framework
├── src/                    # Source files
│   ├── data/
│   ├── indexes/
│   ├── benchmark/
│   └── main.cpp               # Main test program
├── tests/                  # Unit tests
├── tools/                  # Data processing tools
└── data/                   # Data directory (to be created)
```

## Prerequisites

### Required
- **C++17** compatible compiler (GCC 7+, Clang 5+, or MSVC 2017+)
- **CMake 3.15+**
- **Boost 1.70+** (for Boost.Geometry R-tree)

### Optional
- **OpenMP** (for parallel processing)
- **Python 3** (for data preprocessing and result visualization)

## Installation

### 1. Install Dependencies

#### macOS
```bash
brew install cmake boost
```

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install cmake libboost-all-dev
```

### 2. Build the Project

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build
make -j$(nproc)

# Run tests
make test
```

### 3. Verify Installation

```bash
# Run the basic test program
./bin/flood_index
```

You should see output showing successful creation and testing of data points.

## Quick Start

### 1. Prepare Data

Download NYC TLC taxi data:
```bash
# Create data directory
mkdir -p data/nyc_taxi

# Download one month of data (example: January 2024)
# Visit: https://www.nyc.gov/site/tlc/about/tlc-trip-record-data.page
# Download the Parquet or CSV file to data/nyc_taxi/
```

### 2. Process Data

```bash
# Convert NYC taxi data to binary format
./bin/process_data data/nyc_taxi/input.csv data/nyc_taxi/processed.bin
```

### 3. Run Benchmark

```bash
# Run full benchmark suite
./bin/run_benchmark data/nyc_taxi/processed.bin results/
```

## Development Roadmap

### Phase 1: Infrastructure (Week 1) ✓
- [x] Project structure and CMake setup
- [x] Base classes and interfaces
- [x] Data loading framework
- [x] Basic compilation test

### Phase 2: Baseline Indexes (Week 2)
- [ ] Implement R*-tree wrapper (Boost.Geometry)
- [ ] Implement k-d tree
- [ ] Implement Z-order curve index
- [ ] Test baseline indexes

### Phase 3: Flood Index (Week 2-3)
- [ ] Implement flattening technique
- [ ] Implement cost model
- [ ] Implement learning algorithm
- [ ] Test Flood index

### Phase 4: Benchmarking (Week 4)
- [ ] Implement workload generators
- [ ] Run experiments
- [ ] Collect performance metrics
- [ ] Generate comparison plots

### Phase 5: Analysis & Report (Week 5)
- [ ] Analyze results
- [ ] Write final report
- [ ] Prepare presentation

## Usage Examples

### Create Sample Data
```cpp
#include "data/data_point.h"

std::vector<double> coords = {-74.0, 40.7, 1234567890.0};
DataPoint point(coords, 0);
```

### Define Query Range
```cpp
#include "data/data_point.h"

std::vector<double> min_bounds = {-74.1, 40.6, 0.0};
std::vector<double> max_bounds = {-73.9, 40.8, 1000.0};
QueryRange range(min_bounds, max_bounds);
```

### Load Data
```cpp
#include "data/data_loader.h"

DataLoader loader;
auto data = loader.loadFromBinary("data/processed.bin");
auto stats = loader.computeStats(data);
stats.print();
```

## Testing

```bash
# Run all tests
cd build
./bin/run_tests

# Run with verbose output
./bin/run_tests --verbose
```

## Performance Metrics

The benchmark framework measures:
- **Build Time**: Time to construct the index (milliseconds)
- **Index Size**: Memory footprint (megabytes)
- **Query Time**: Average, median, P95, P99 query latency (milliseconds)
- **Scan Overhead**: Ratio of scanned records to returned records

## Data Format

### Input CSV Format
```
pickup_longitude,pickup_latitude,pickup_datetime,passenger_count,trip_distance
-73.98,40.75,2024-01-01 00:00:00,1,2.5
...
```

### Binary Format
- Header: `num_points (size_t)`, `dimensions (size_t)`
- For each point: `id (uint64_t)`, `coordinates (double[])`

## Contributing

This is a course project for CIS 6500 at UPenn. 

## References

1. Kraska, T., et al. (2020). "Learning Multi-dimensional Indexes." SIGMOD.
2. Boost.Geometry Documentation: https://www.boost.org/doc/libs/release/libs/geometry/
3. NYC TLC Trip Record Data: https://www.nyc.gov/site/tlc/about/tlc-trip-record-data.page

## License

This project is for educational purposes only.

## Contact

For questions or issues, please contact the project maintainer.

