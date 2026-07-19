#ifndef CACHESIM_CACHE_HPP
#define CACHESIM_CACHE_HPP
#include <vector>
#include "cachesim/cache_set.hpp"
#include "cachesim/address.hpp"
#include "cachesim/config.hpp"
namespace cachesim {

class Cache {
public:
    Cache(const CacheConfig& cfg, uint64_t block_size, PolicyKind policy_kind,
          WritePolicy write_policy, AllocPolicy alloc_policy)
        : block_size_(block_size),
          num_sets_(cfg.size_bytes / (block_size * cfg.associativity)),
          write_policy_(write_policy),
          alloc_policy_(alloc_policy) {
        for (size_t i = 0; i < num_sets_; ++i) {
            sets_.emplace_back(cfg.associativity, policy_kind);
        }
    }

    // evicted_dirty: did this access evict a line that had unsaved writes?
    // wrote_to_memory: did this access cause an immediate write to memory
    // (always true for write-through writes and no-write-allocate misses;
    // never true for write-back hits, since those just mark dirty instead)
    bool access(uint64_t addr, bool is_write, bool& evicted_dirty, bool& wrote_to_memory) {
        evicted_dirty = false;
        wrote_to_memory = false;

        Decomposed d = decompose_address(addr, block_size_, num_sets_);
        CacheSet& set = sets_[d.index];

        auto way = set.find(d.tag);
        if (way.has_value()) {
            // Hit.
            set.policy().on_access(*way);
            if (is_write) {
                if (write_policy_ == WritePolicy::WriteBack) {
                    set.line(*way).dirty = true;
                } else {
                    // Write-through: memory is updated right now, so this
                    // line is never "ahead of" memory -- never dirty.
                    wrote_to_memory = true;
                }
            }
            return true;
        }

        // Miss.
        if (is_write && alloc_policy_ == AllocPolicy::NoWriteAllocate) {
            // Don't touch the cache at all -- write straight through.
            wrote_to_memory = true;
            return false;
        }

        // Write-allocate (or a read miss, which always allocates).
        size_t victim_way = set.choose_victim();
        CacheLine& victim_line = set.line(victim_way);
        evicted_dirty = victim_line.valid && victim_line.dirty;

        victim_line.tag   = d.tag;
        victim_line.valid = true;
        victim_line.dirty = is_write && (write_policy_ == WritePolicy::WriteBack);
        if (is_write && write_policy_ == WritePolicy::WriteThrough) {
            wrote_to_memory = true;
        }

        set.policy().on_insert(victim_way);
        return false;
    }

private:
    uint64_t     block_size_;
    size_t       num_sets_;
    WritePolicy  write_policy_;
    AllocPolicy  alloc_policy_;
    std::vector<CacheSet> sets_;
};

}  // namespace cachesim
#endif  // CACHESIM_CACHE_HPP