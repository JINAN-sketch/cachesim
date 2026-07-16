#ifndef CACHESIM_CONFIG_HPP
#define CACHESIM_CONFIG_HPP

#include <cstdint>
#include <string>
#include <stdexcept>

#include "cachesim/types.hpp" // brings in PolicyKind, WritePolicy, AllocPolicy

namespace cachesim{
    // Returns true if n is a power of two (1, 2, 4, 8, 16, ...).
    // Bit trick: a power of two has exactly one bit set, e.g. 8 = 0b1000.
    // Subtracting 1 flips that bit and sets all lower bits: 7 = 0b0111.
    // ANDing them together gives 0 only when n was a power of two.
    inline bool is_power_of_two(uint64_t n){
        return n != 0 && (n&(n-1)) == 0;
    }

    struct CacheConfig{
        uint64_t size_bytes;
        size_t associativity;

        // Constructor: runs validation the moment a CacheConfig is created.
        // If anything is invalid, it throws immediately — an invalid
        // CacheConfig object can never exist past construction.
        CacheConfig(uint64_t size_bytes_, size_t associativity_)
        : size_bytes(size_bytes_), associativity(associativity_){
            if(!is_power_of_two(size_bytes)){
                throw std::invalid_argument("cache size must be a power of two");
            }
            if(associativity == 0){
                throw std::invalid_argument("associativity must be at least 1");
            }
        }
    };
    struct SimConfig{
        CacheConfig  l1;             // L1 cache config (nested struct as a member)
        CacheConfig  l2;             // L2 cache config
        uint64_t     block_size;     // shared block/line size for both levels
        PolicyKind   policy;         // LRU / FIFO / PLRU
        WritePolicy  write_policy;   // WriteBack / WriteThrough
        AllocPolicy  alloc_policy;   // WriteAllocate / NoWriteAllocate
        uint64_t     mem_latency;    // cycles charged on a main-memory access
        std::string  trace_path;     // path to the trace file to replay

        SimConfig(CacheConfig l1_, CacheConfig l2_, uint64_t block_size_,PolicyKind policy_, WritePolicy write_policy_,
                    AllocPolicy alloc_policy_, uint64_t mem_latency_,
                    std::string trace_path_)
              : l1(l1_), l2(l2_), block_size(block_size_), policy(policy_),
                write_policy(write_policy_), alloc_policy(alloc_policy_),
                mem_latency(mem_latency_),
                trace_path(std::move(trace_path_)){

                    if (!is_power_of_two(block_size)) {
                        throw std::invalid_argument("block size must be a power of two");
                    }

                    // Number of cache lines = total size / block size. If associativity
                    // (ways per set) exceeds this, there literally aren't enough lines
                    // to form even one full set — an impossible configuration.
                    uint64_t l1_lines = l1.size_bytes / block_size;
                    if (l1.associativity > l1_lines) {
                        throw std::invalid_argument("L1 associativity exceeds number of lines");
                    }

                    uint64_t l2_lines = l2.size_bytes / block_size;
                    if (l2.associativity > l2_lines) {
                        throw std::invalid_argument("L2 associativity exceeds number of lines");
                    }

                    //number of ways must be a power of two
                    if (policy == PolicyKind::PLRU) {
                        if (!is_power_of_two(l1.associativity) || !is_power_of_two(l2.associativity)) {
                            throw std::invalid_argument("PLRU requires power-of-2 associativity");
                        }
                    }

                    if (trace_path.empty()) {
                        throw std::invalid_argument("trace path must not be empty");
                    }
                }

    };
}
#endif