#include "rrip.h"

void update_rrpv(std::vector<int>& rrpv, std::size_t way, Classification cls, bool is_hit) {
    if (cls == Classification::CACHE_AVERSE) {
        rrpv[way] = 7;
    } else {
        rrpv[way] = 0;
    }
}

std::size_t find_victim(std::vector<int>& rrpv) {
    while (true) {
        for (std::size_t i = 0; i < rrpv.size(); i++) {
            if (rrpv[i] == 7) {
                return i;
            }
        }
        for (std::size_t i = 0; i < rrpv.size(); i++) {
            if (rrpv[i] < 7) {
                rrpv[i]++;
            }
        }
    }
}