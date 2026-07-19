#ifndef CACHESIM_CACHE_HPP
#define CACHESIM_CACHE_HPP
#include <vector>
#include "cachesim/cache_set.hpp"
#include "cachesim/address.hpp"
#include "cachesim/config.hpp"

namespace cachesim{

// One cache level (L1 or L2 use this same class, just different configs).
class Cache {
public:
     Cache(const CacheConfig& cfg, uint64_t block_size, PolicyKind policy_kind)
        : block_size_(block_size),
          num_sets_(cfg.size_bytes / (block_size * cfg.associativity)){
            for(size_t i = 0; i <num_sets_; ++i){
                sets_.emplace_back(cfg.associativity,policy_kind);
            }
        }

        bool access(uint64_t addr, bool is_write, bool& evicted_dirty){
            Decomposed d = decompose_address(addr,block_size_,num_sets_);
            CacheSet& set = sets_[d.index];

            auto way = set.find(d.tag);
            if(way.has_value()){
                //hit
                set.policy().on_access(*way);
                if(is_write){
                    set.line(*way).dirty = true;
                }
                evicted_dirty = false;
                return true;
            }

            // Miss: pick a victim and evict it.
            size_t victim_way = set.choose_victim();
            CacheLine& victim_line = set.line(victim_way);
            evicted_dirty = victim_line.valid && victim_line.dirty;

            // Install the new line in the victim's spot.
            victim_line.tag   = d.tag;
            victim_line.valid = true;
            victim_line.dirty = is_write;

            set.policy().on_insert(victim_way);
            return false;
        }
private:
    uint64_t block_size_;
    size_t   num_sets_;
    std::vector<CacheSet> sets_;
};
}
#endif