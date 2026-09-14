#include "hawkeye.h"
#include <algorithm>
#include "cache.h"
#include "champsim.h"

hawkeye::hawkeye(CACHE* cache) : hawkeye(cache, cache->NUM_SET, cache->NUM_WAY) {}

hawkeye::hawkeye(CACHE* cache, long sets, long ways)
    : replacement(cache),
      optgen(sets, ways),
      predictor(),
      rrpv(sets, std::vector<int>(ways, 0))
{
}

long hawkeye::find_victim(uint32_t cpu, uint64_t instr, long set, const champsim::cache_block* current_set, champsim::address ip, champsim::address ful_ad, access_type type) {
  std::size_t set_idx = set;
  for (std::size_t w = 0; w < rrpv.at(set_idx).size(); w++) {
    if (!current_set[w].valid) {
      return static_cast<long>(w);
    }
  }
  std::size_t victim_way = ::find_victim(rrpv.at(set_idx));
  return static_cast<long>(victim_way);
}

void hawkeye::replacement_cache_fill(uint32_t cpu, long set, long way, champsim::address ful_ad, champsim::address ip, champsim::address victim_addr, access_type type) {
  bool friendly = predictor.predict(ip.to<uint64_t>());
  Classification cls = friendly ? Classification::CACHE_FRIENDLY : Classification::CACHE_AVERSE;
  update_rrpv(rrpv.at(set), way, cls, false);
}

void hawkeye::update_replacement_state(uint32_t cpu, long set, long way, champsim::address ful_ad, champsim::address ip, champsim::address victim_addr, access_type type, uint8_t hit) {
    champsim::block_number blc_ad{ful_ad};
    uint64_t block = blc_ad.to<uint64_t>();
    bool had_recent = optgen.has_recent_access(static_cast<std::size_t>(set), block);
    bool opt_hit = optgen.access(static_cast<std::size_t>(set), block);
    if (had_recent && last_pc.count(block)) {
      predictor.train(last_pc[block], opt_hit);
    }
    last_pc[block] = ip.to<uint64_t>();
    if (hit) {
        bool friendly = predictor.predict(ip.to<uint64_t>());
        Classification cls = friendly ? Classification::CACHE_FRIENDLY : Classification::CACHE_AVERSE;
        update_rrpv(rrpv.at(set), way, cls, true);
    }
}
