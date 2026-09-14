#include "hawkeye.h"
#include <algorithm>
#include "cache.h"

hawkeye::hawkeye(CACHE* cache) : hawkeye(cache, cache->NUM_SET, cache->NUM_WAY) {}

hawkeye::hawkeye(CACHE* cache, long sets, long ways)
    : replacement(cache), optgen(static_cast<std::size_t>(sets), static_cast<std::size_t>(ways)), predictor(),
      rrpv(static_cast<std::size_t>(sets), std::vector<int>(static_cast<std::size_t>(ways), 0))
{
}

long hawkeye::find_victim(uint32_t triggering_cpu, uint64_t instr_id, long set, const champsim::cache_block* current_set, champsim::address ip,
                          champsim::address full_addr, access_type type)
{
  std::size_t victim_way = ::find_victim(rrpv.at(static_cast<std::size_t>(set)));
  return static_cast<long>(victim_way);
}

void hawkeye::replacement_cache_fill(uint32_t triggering_cpu, long set, long way, champsim::address full_addr, champsim::address ip,
                                     champsim::address victim_addr, access_type type)
{
  bool friendly = predictor.predict(ip.to<uint64_t>());
  Classification cls = friendly ? Classification::CACHE_FRIENDLY : Classification::CACHE_AVERSE;

  update_rrpv(rrpv.at(static_cast<std::size_t>(set)), static_cast<std::size_t>(way), cls, /*is_hit=*/false);
}

void hawkeye::update_replacement_state(uint32_t triggering_cpu, long set, long way, champsim::address full_addr, champsim::address ip,
                                       champsim::address victim_addr, access_type type, uint8_t hit)
{
  if (type == access_type::WRITE) {
    return;
  }

  bool opt_hit = optgen.access(static_cast<std::size_t>(set), full_addr.to<uint64_t>());
  predictor.train(ip.to<uint64_t>(), opt_hit);

  if (hit) {
    bool friendly = predictor.predict(ip.to<uint64_t>());
    Classification cls = friendly ? Classification::CACHE_FRIENDLY : Classification::CACHE_AVERSE;
    update_rrpv(rrpv.at(static_cast<std::size_t>(set)), static_cast<std::size_t>(way), cls, /*is_hit=*/true);
  }
}
