#ifndef CACHESIM_TYPES_HPP
#define CACHESIM_TYPES_HPP

#include <cstdint>

namespace cachesim{
    struct MemAccess{
        uint64_t addr; // mem being accessed
        bool is_write; //true means store/write, false is load/read
    };
    struct CacheResult{
        bool hit;  // true if access was a hit
        uint64_t cycles; // latency charged for this access
    };

// enum class (not plain enum) means you must write WritePolicy::WriteBack
// everywhere — it will NOT silently convert to an int, and it can't be
// compared against an unrelated enum by accident. Plain "enum" allows both
// of those footguns; enum class closes them at compile time for free.
    enum class WritePolicy { WriteBack, WriteThrough };
    enum class AllocPolicy { WriteAllocate, NoWriteAllocate };
    enum class PolicyKind  { LRU, FIFO, PLRU };

}
#endif
