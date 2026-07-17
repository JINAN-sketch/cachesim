#ifndef CACHESIM_CACHE_SET_HPP
#define CACHESIM_CACHE_SET_HPP
#include <vector>
#include <optional>
#include <memory>
#include "cachesim/cache_line.hpp"
#include "cachesim/replacement_policy.hpp"
namespace cachesim {

// One set in the cache: a handful of ways (lines) plus the policy that
// decides which way to evict when the set is full.
class CacheSet {
public:
    CacheSet(size_t ways, PolicyKind policy_kind)
        : lines_(ways), policy_(make_policy(policy_kind, ways)) {}

    // Looks for `tag` among valid lines. Returns the way index if found,
    // or an empty optional if not present (no -1 sentinel needed).
    std::optional<size_t> find(uint64_t tag) const {
        for (size_t w = 0; w < lines_.size(); ++w) {
            if (lines_[w].valid && lines_[w].tag == tag) {
                return w;
            }
        }
        return std::nullopt;
    }

    size_t choose_victim() const { return policy_->victim(); }

    CacheLine& line(size_t way) { return lines_[way]; }

    ReplacementPolicy& policy() { return *policy_; }

private:
    std::vector<CacheLine> lines_;
    std::unique_ptr<ReplacementPolicy> policy_;
};

}  // namespace cachesim
#endif  // CACHESIM_CACHE_SET_HPP