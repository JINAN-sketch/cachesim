#include <gtest/gtest.h>
#include "cachesim/memory_hierarchy.hpp"

using namespace cachesim;

TEST(MemoryHierarchy, L1MissTriggersL2AndDirtyEvictionWritesBack) {
    SimConfig cfg(
        CacheConfig(32, 1),
        CacheConfig(64, 2),
        16,
        PolicyKind::LRU,
        WritePolicy::WriteBack,
        AllocPolicy::WriteAllocate,
        100,
        "dummy.trace"
    );
    MemoryHierarchy mh(cfg);

    EXPECT_FALSE(mh.access(MemAccess{0x00, true}));
    EXPECT_EQ(mh.l1_misses(), 1u);
    EXPECT_EQ(mh.l2_misses(), 1u);

    EXPECT_TRUE(mh.access(MemAccess{0x00, false}));
    EXPECT_EQ(mh.l1_hits(), 1u);
    EXPECT_EQ(mh.l2_hits(), 0u);
    EXPECT_EQ(mh.l2_misses(), 1u);

    EXPECT_FALSE(mh.access(MemAccess{0x20, false}));
    EXPECT_EQ(mh.l1_misses(), 2u);
    EXPECT_EQ(mh.l2_hits(), 1u);
    EXPECT_EQ(mh.l2_misses(), 2u);
}