#ifndef BASE_INDEX_H
#define BASE_INDEX_H

#include "data/data_point.h"
#include <vector>
#include <string>
#include <chrono>

namespace flood {

/**
 * Performance metrics for an index
 */
struct IndexMetrics {
    double build_time_ms;      // Time to build the index (milliseconds)
    double index_size_mb;      // Size of the index in memory (megabytes)
    double avg_query_time_ms;  // Average query time (milliseconds)
    double scan_overhead;      // Ratio of scanned records to returned records
    size_t total_queries;      // Number of queries executed
    
    void print() const;
    std::string toCSV() const;
};

/**
 * BaseIndex: Abstract base class for all spatial indexes
 * All index implementations must inherit from this class
 */
class BaseIndex {
public:
    virtual ~BaseIndex() = default;
    
    /**
     * Build the index from a set of data points
     * @param data Vector of data points to index
     */
    virtual void build(const std::vector<DataPoint>& data) = 0;
    
    /**
     * Execute a range query on the index
     * @param range The query range
     * @return Vector of data points within the range
     */
    virtual std::vector<DataPoint> query(const QueryRange& range) = 0;
    
    /**
     * Get the size of the index in megabytes
     */
    virtual double getIndexSize() const = 0;
    
    /**
     * Get the time taken to build the index (milliseconds)
     */
    virtual double getBuildTime() const { return build_time_ms_; }
    
    /**
     * Get the name of the index (e.g., "Flood", "R*-tree")
     */
    virtual std::string getName() const = 0;
    
    /**
     * Get metrics about the index
     */
    IndexMetrics getMetrics() const { return metrics_; }
    
    /**
     * Reset metrics
     */
    void resetMetrics();

protected:
    // Metrics tracking
    IndexMetrics metrics_;
    double build_time_ms_ = 0.0;
    size_t data_size_ = 0;
    
    // Helper function to measure time
    class Timer {
    public:
        Timer() : start_(std::chrono::high_resolution_clock::now()) {}
        
        double elapsed() const {
            auto end = std::chrono::high_resolution_clock::now();
            return std::chrono::duration<double, std::milli>(end - start_).count();
        }
        
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> start_;
    };
};

} // namespace flood

#endif // BASE_INDEX_H

