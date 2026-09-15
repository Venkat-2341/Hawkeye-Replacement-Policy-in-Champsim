#include "../replacement/hawkeye/optgen.h"
#include <iostream>
#include <vector>
#include <utility>
#include <cstdint>

int main() {
    OPTgen opt(1, 2);

    // TEST_VECTOR_START
    std::vector<std::pair<std::size_t, uint64_t>> accesses = {};
    // TEST_VECTOR_END

    int hits = 0;
    for (auto& [set_idx, addr] : accesses) {
            bool hit = opt.access(set_idx, addr);
            std::cout << std::hex << addr << std::dec << ": " << (hit ? "HIT" : "MISS") << "\n";
            if (hit) {
                hits++;
                // std::cout << std::hex << addr << std:: endl;
            }
    }

    std::cout << "TOTAL HITS: " << hits << "\n";
}
