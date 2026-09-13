#include "predictor.h"
using namespace std;

HawkeyePredictor::HawkeyePredictor(std::size_t num_entries, int counter_bits) {
    num_entries_ = num_entries;
    counter_bits_ = counter_bits;

    table = vector<int> (num_entries, (1 << (counter_bits_  - 1)));
}

size_t HawkeyePredictor::find_hash(uint64_t pc) const {
    
    // we must get a valid entry: [0, num_entries_)
    uint64_t hashed = pc ^ (pc >> 12);
    std::size_t num_bits = 0;
    std::size_t n = num_entries_;
    while (n > 1) {
        n /= 2;
        num_bits++;
    }
    uint64_t mask = (static_cast<uint64_t>(1) << num_bits) - 1;
    return static_cast<std::size_t>(hashed & mask);
}

void HawkeyePredictor::train(uint64_t pc, bool opt_hit) {
    size_t index = find_hash(pc);

    int max_val = (1 << counter_bits_) - 1;
    if (opt_hit) {
        table[index]++;
        table[index] = min(table[index], max_val);
    }
    else {
        table[index]--;
        table[index] = max(table[index], 0);
    }
}

bool HawkeyePredictor::predict(uint64_t pc) const {
    size_t index = find_hash(pc);

    int max_half = (1 << (counter_bits_ - 1));
    if (table[index] >= max_half) return true; // cache friendly
    else return false; // cache averse
}

int HawkeyePredictor::get_counter(uint64_t pc) const {
    size_t index = find_hash(pc);
    return table[index];
}

