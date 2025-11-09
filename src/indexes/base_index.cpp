#include "indexes/base_index.h"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace flood {

void IndexMetrics::print() const {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Index Metrics:\n";
    std::cout << "  Build Time: " << build_time_ms << " ms\n";
    std::cout << "  Index Size: " << index_size_mb << " MB\n";
    std::cout << "  Avg Query Time: " << avg_query_time_ms << " ms\n";
    std::cout << "  Scan Overhead: " << scan_overhead << "x\n";
    std::cout << "  Total Queries: " << total_queries << "\n";
}

std::string IndexMetrics::toCSV() const {
    std::ostringstream oss;
    oss << build_time_ms << ","
        << index_size_mb << ","
        << avg_query_time_ms << ","
        << scan_overhead << ","
        << total_queries;
    return oss.str();
}

void BaseIndex::resetMetrics() {
    metrics_ = IndexMetrics();
    build_time_ms_ = 0.0;
}

} // namespace flood
