#ifndef HAWKEYE_OPTGEN_H
#define HAWKEYE_OPTGEN_H

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

class OPTgen {
public:
    // num_sets: number of cache sets tracked independently
    // associativity: W, the cache associativity (occupancy vector cap)
    // history_multiplier: length of tracked history, in units of the set's
    //                      capacity (paper uses 8x; see Figure 2). Default 8.
    OPTgen(std::size_t num_sets, std::size_t associativity,
           std::size_t history_multiplier = 8);
 
    // Processes one access to `address`, mapped to set `set_idx`
    bool access(std::size_t set_idx, uint64_t address);
 
private:
    std::size_t num_sets_;
    std::size_t associativity_;      // W
    std::size_t history_length_;     // 8W (or history_multiplier * W)
 
    // occupancy_[set_idx][t % history_length_] has the number of live reuse intervals overlapping time = t. 
    std::vector<std::vector<std::size_t>> occupancy_;
    std::vector<std::size_t> current_time_; // local to each set
    std::vector<std::unordered_map<uint64_t, std::size_t>> last_access_time_;
};  

#endif