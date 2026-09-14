#include "optgen.h"
using namespace std;

OPTgen::OPTgen(size_t num_sets, size_t associativity, size_t history_multiplier) {
    num_sets_ = num_sets;
    associativity_ = associativity;
    history_length_ = history_multiplier * associativity;

    occupancy_ = vector<vector<size_t>>(num_sets_, vector<size_t>(history_length_, 0));
    current_time_ = vector<size_t>(num_sets_, 0);
    last_access_time_ = vector<unordered_map<uint64_t, size_t>>(num_sets_);
}

bool OPTgen::access(size_t set_idx, uint64_t address) {
    // return true if its hit and false for a miss 
    occupancy_[set_idx][current_time_[set_idx] % history_length_] = 0;
    
    unordered_map<uint64_t, size_t> &mp = last_access_time_[set_idx];
    if (!mp.count(address)) {
        // cold miss: this is first time im seeing this address

        mp[address] = current_time_[set_idx];
        current_time_[set_idx]++;

        return false; 
    }

    else {

        // the max number of entries i can keep in a set is 'associativity'
        // ill check if i could have kept this address in the cache from last access time to now

        size_t prev_time = mp[address];
        size_t curr_time = current_time_[set_idx];

        if (curr_time - prev_time >= history_length_) {
            // since we have only a limited history, we cant resolve this
            // Assumption: we are treating this as a miss

            mp[address] = curr_time;
            current_time_[set_idx]++;

            return false;
        }

        bool can_assign = true;
        for (size_t t = prev_time; t < curr_time; t++) {
            if (occupancy_[set_idx][t % history_length_] >= associativity_) {
                can_assign = false;
                break;
            }
        }

        mp[address] = curr_time;
        current_time_[set_idx]++;

        if (can_assign) {
            // we increase the occupunancy since now this address will be live from [prev, cur) time
            for (size_t t = prev_time; t < curr_time; t++) {
                occupancy_[set_idx][t % history_length_]++;
            }
            return true;
        }
        else return false;
    }
}