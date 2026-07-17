#ifndef CACHESIM_REPLACEMENT_POLICY_HPP
#define CACHESIM_REPLACEMENT_POLICY_HPP

#include <cstddef>
#include <memory>
#include <string_view>

#include "cachesim/types.hpp"

namespace cachesim {

// Abstract interface (Strategy pattern). Every concrete policy
// (LRU, FIFO, PLRU) implements this. CacheSet only ever talks to a
// ReplacementPolicy*, never to a concrete type directly.
class ReplacementPolicy {
public:
    // Virtual destructor: required whenever a class will be deleted
    // through a base-class pointer (which unique_ptr<ReplacementPolicy>
    // does). Without "virtual" here, deleting a derived object through
    // a base pointer would skip the derived class's destructor — a real
    // bug, not just a style issue.
    virtual ~ReplacementPolicy() = default;

    // Pure virtual ("= 0"): no body here, every derived class MUST
    // provide one. This is what makes ReplacementPolicy "abstract" —
    // you can never create a plain ReplacementPolicy object directly.
    virtual size_t victim() const = 0;         // which way to evict next
    virtual void on_access(size_t way) = 0;    // called on a hit
    virtual void on_insert(size_t way) = 0;    // called when a line is filled
    virtual std::string_view name() const = 0; // e.g. "LRU", for debugging/reports
};

// Factory function: given a policy kind and the number of ways, returns
// the right concrete policy, wrapped in a unique_ptr. Declared here,
// defined in policies.cpp once the concrete policies exist (Day 5/11).
std::unique_ptr<ReplacementPolicy> make_policy(PolicyKind kind, size_t ways);

}  // namespace cachesim

#endif  // CACHESIM_REPLACEMENT_POLICY_HPP