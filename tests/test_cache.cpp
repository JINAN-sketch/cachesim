#include <gtest/gtest.h>
#include "cachesim/cache.hpp"

using namespace cachesim;

TEST(Cache, HitsMissesAndEviction) {
    CacheConfig cfg(128, 2);
    Cache cache(cfg, 16, PolicyKind::LRU, WritePolicy::WriteBack, AllocPolicy::WriteAllocate);

    bool evicted_dirty, wrote_to_memory;
    uint64_t evicted_addr;

    EXPECT_FALSE(cache.access(0x00, true, evicted_dirty, wrote_to_memory, evicted_addr));
    EXPECT_FALSE(evicted_dirty);

    EXPECT_TRUE(cache.access(0x00, false, evicted_dirty, wrote_to_memory, evicted_addr));

    EXPECT_FALSE(cache.access(0x40, false, evicted_dirty, wrote_to_memory, evicted_addr));
    EXPECT_FALSE(evicted_dirty);

    EXPECT_TRUE(cache.access(0x00, false, evicted_dirty, wrote_to_memory, evicted_addr));

    EXPECT_FALSE(cache.access(0x80, true, evicted_dirty, wrote_to_memory, evicted_addr));
    EXPECT_FALSE(evicted_dirty);

    // This eviction is dirty (the 0x00 line, written in step 1) -- confirm
    // evicted_addr correctly reconstructs the original address.
    EXPECT_FALSE(cache.access(0xC0, false, evicted_dirty, wrote_to_memory, evicted_addr));
    EXPECT_TRUE(evicted_dirty);
    EXPECT_EQ(evicted_addr, 0x00u);
}

TEST(Cache, WriteThroughNeverGoesDirty) {
    CacheConfig cfg(128, 2);
    Cache cache(cfg, 16, PolicyKind::LRU, WritePolicy::WriteThrough, AllocPolicy::WriteAllocate);

    bool evicted_dirty, wrote_to_memory;
    uint64_t evicted_addr;

    EXPECT_FALSE(cache.access(0x00, true, evicted_dirty, wrote_to_memory, evicted_addr));
    EXPECT_TRUE(wrote_to_memory);

    EXPECT_TRUE(cache.access(0x00, true, evicted_dirty, wrote_to_memory, evicted_addr));
    EXPECT_TRUE(wrote_to_memory);

    cache.access(0x40, true, evicted_dirty, wrote_to_memory, evicted_addr);
    EXPECT_FALSE(cache.access(0x80, true, evicted_dirty, wrote_to_memory, evicted_addr));
    EXPECT_FALSE(evicted_dirty);
}

TEST(Cache, NoWriteAllocateSkipsCacheOnWriteMiss) {
    CacheConfig cfg(128, 2);
    Cache cache(cfg, 16, PolicyKind::LRU, WritePolicy::WriteBack, AllocPolicy::NoWriteAllocate);

    bool evicted_dirty, wrote_to_memory;
    uint64_t evicted_addr;

    EXPECT_FALSE(cache.access(0x00, true, evicted_dirty, wrote_to_memory, evicted_addr));
    EXPECT_TRUE(wrote_to_memory);
    EXPECT_FALSE(evicted_dirty);

    EXPECT_FALSE(cache.access(0x00, true, evicted_dirty, wrote_to_memory, evicted_addr));
}
