#include "indexes/base_index.h"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace flood {

void IndexMetrics::print() const {
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "=== Index Metrics ===" << std::endl;
    std::cout << "Build Time: " << build_time_ms << " ms" << std::endl;
    std::cout << "Index Size: " << index_size_mb << " MB" << std::endl;
    std::cout << "Avg Query Time: " << avg_query_time_ms << " ms" << std::endl;
    std::cout << "Scan Overhead: " << scan_overhead << "x" << std::endl;
    std::cout << "Total Queries: " << total_queries << std::endl;
}

std::string IndexMetrics::toCSV() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3);
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

