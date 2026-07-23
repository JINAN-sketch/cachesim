#include <gtest/gtest.h>
#include "cachesim/memory_hierarchy.hpp"

using namespace cachesim;

// A deeper hierarchy trace: forces evictions in BOTH L1 and L2, including
// L2 evicting its own dirty write-back data. Hand-traced before writing --
// see chat history / DESIGN.md for the full step-by-step.
TEST(MemoryHierarchy, TwoLevelEvictionAndWriteBackChain) {
    SimConfig cfg(
        CacheConfig(32, 1),  // L1: 2 sets, 1-way
        CacheConfig(32, 1),  // L2: 2 sets, 1-way (same size, fills equally fast)
        16,
        PolicyKind::LRU,
        WritePolicy::WriteBack,
        AllocPolicy::WriteAllocate,
        100,
        "dummy.trace"
    );
    MemoryHierarchy mh(cfg);

    // 1. write(0x00): fresh miss everywhere.
    EXPECT_FALSE(mh.access(MemAccess{0x00, true}));

    // 2. write(0x10): fresh miss, different set. Both caches now full.
    EXPECT_FALSE(mh.access(MemAccess{0x10, true}));

    // 3. write(0x20): L1 evicts dirty tag-0 (set 0). Write-back HITS in L2
    // (L2 still has tag 0 there). Fetching tag 1 is then an L2 MISS,
    // which evicts L2's now-dirty tag-0 line -- absorbed, nowhere to go.
    EXPECT_FALSE(mh.access(MemAccess{0x20, true}));

    // 4. read(0x30): mirror of step 3, on set 1 instead.
    EXPECT_FALSE(mh.access(MemAccess{0x30, false}));

    // 5. read(0x20): L1 still holds tag 1 from step 3 -- clean hit, no L2 traffic.
    EXPECT_TRUE(mh.access(MemAccess{0x20, false}));

    EXPECT_EQ(mh.l1_hits(), 1u);
    EXPECT_EQ(mh.l1_misses(), 4u);
    EXPECT_EQ(mh.l2_hits(), 2u);
    EXPECT_EQ(mh.l2_misses(), 4u);
}