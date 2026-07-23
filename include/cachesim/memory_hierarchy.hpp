#ifndef CACHESIM_MEMORY_HIERARCHY_HPP
#define CACHESIM_MEMORY_HIERARCHY_HPP
#include "cachesim/cache.hpp"
#include "cachesim/config.hpp"

namespace cachesim{

class MemoryHierarchy{
public:
    explicit MemoryHierarchy(const SimConfig& cfg)
        : l1_(cfg.l1, cfg.block_size, cfg.policy, cfg.write_policy, cfg.alloc_policy),
          l2_(cfg.l2, cfg.block_size, cfg.policy, cfg.write_policy, cfg.alloc_policy) {}

          // Routes one access through L1, then L2 on an L1 miss. Returns true
    // only if it hit in L1 (the fastest possible outcome).
    bool access(const MemAccess& a) {
        l1_accesses_++;
        bool l1_evicted_dirty, l1_wrote_to_memory;
        uint64_t l1_evicted_addr;
        bool l1_hit = l1_.access(a.addr, a.is_write, l1_evicted_dirty,
                                 l1_wrote_to_memory, l1_evicted_addr);
        if(l1_hit){
            l1_hits_++;
            return true;
        }
        l1_misses_++;
        

        // If L1's miss evicted a dirty line, write it back to L2 first --
        // that data would otherwise be lost.
        if (l1_evicted_dirty) {
            access_l2(l1_evicted_addr, /*is_write=*/true);
        }

        // Fetch the new block from L2 (always a read of the block itself,
        // regardless of whether the original access was a write -- the
        // block has to be loaded before the write can apply to it).
        access_l2(a.addr, false);
        return false;
    }
    size_t l1_hits() const { return l1_hits_; }
    size_t l1_misses() const { return l1_misses_; }
    size_t l2_hits() const { return l2_hits_; }
    size_t l2_misses() const { return l2_misses_; }

private:
    void access_l2(uint64_t addr, bool is_write) {
        l2_accesses_++;
        bool evicted_dirty, wrote_to_memory;
        uint64_t evicted_addr;
        bool hit = l2_.access(addr, is_write, evicted_dirty, wrote_to_memory, evicted_addr);
        if (hit) {
            l2_hits_++;
        } else {
            l2_misses_++;
            // A dirty eviction from L2 would go to main memory -- we don't
            // model memory as a real store, so there's nothing further to do;
            // it's simply gone, same as real memory silently absorbing it.
        }
    }

    Cache l1_;
    Cache l2_;
    size_t l1_accesses_ = 0, l1_hits_ = 0, l1_misses_ = 0;
    size_t l2_accesses_ = 0, l2_hits_ = 0, l2_misses_ = 0;
};

}
#endif