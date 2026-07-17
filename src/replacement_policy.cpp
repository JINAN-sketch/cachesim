#include "cachesim/replacement_policy.hpp"
#include "cachesim/policies/lru_policy.hpp"
#include <stdexcept>

namespace cachesim {

// Turns a PolicyKind into the right concrete policy object.
// FIFO and PLRU aren't written yet (Day 11), so they fall through
// to the "not implemented" error for now — LRU is the only real case.
std::unique_ptr<ReplacementPolicy> make_policy(PolicyKind kind, size_t ways) {
    switch (kind) {
        case PolicyKind::LRU:
            return std::make_unique<LruPolicy>(ways);
        case PolicyKind::FIFO:
            throw std::invalid_argument("FIFO policy not implemented yet");
        case PolicyKind::PLRU:
            throw std::invalid_argument("PLRU policy not implemented yet");
    }
    throw std::invalid_argument("unknown policy kind");
}

}  // namespace cachesim