#include <gtest/gtest.h>
#include "cachesim/cache.hpp"

using namespace cachesim;

TEST(Cache, HitsMissesAndEviction) {
    CacheConfig cfg(128, 2);  // 128 bytes, 2-way
    Cache cache(cfg, 16, PolicyKind::LRU, WritePolicy::WriteBack, AllocPolicy::WriteAllocate);

    bool evicted_dirty, wrote_to_memory;

    EXPECT_FALSE(cache.access(0x00, true, evicted_dirty, wrote_to_memory));
    EXPECT_FALSE(evicted_dirty);

    EXPECT_TRUE(cache.access(0x00, false, evicted_dirty, wrote_to_memory));

    EXPECT_FALSE(cache.access(0x40, false, evicted_dirty, wrote_to_memory));
    EXPECT_FALSE(evicted_dirty);

    EXPECT_TRUE(cache.access(0x00, false, evicted_dirty, wrote_to_memory));

    EXPECT_FALSE(cache.access(0x80, true, evicted_dirty, wrote_to_memory));
    EXPECT_FALSE(evicted_dirty);

    EXPECT_FALSE(cache.access(0xC0, false, evicted_dirty, wrote_to_memory));
    EXPECT_TRUE(evicted_dirty);
}

TEST(Cache, WriteThroughNeverGoesDirty) {
    CacheConfig cfg(128, 2);
    Cache cache(cfg, 16, PolicyKind::LRU, WritePolicy::WriteThrough, AllocPolicy::WriteAllocate);

    bool evicted_dirty, wrote_to_memory;

    // Write miss, write-allocate: line gets installed AND written through.
    EXPECT_FALSE(cache.access(0x00, true, evicted_dirty, wrote_to_memory));
    EXPECT_TRUE(wrote_to_memory);

    // Write hit: still writes through every time, never just marks dirty.
    EXPECT_TRUE(cache.access(0x00, true, evicted_dirty, wrote_to_memory));
    EXPECT_TRUE(wrote_to_memory);

    // Force an eviction of this line later -- it should never have gone dirty.
    cache.access(0x40, true, evicted_dirty, wrote_to_memory);      // fills way 1
    EXPECT_FALSE(cache.access(0x80, true, evicted_dirty, wrote_to_memory)); // evicts way 0 (tag 0)
    EXPECT_FALSE(evicted_dirty);  // never dirty under write-through
}

TEST(Cache, NoWriteAllocateSkipsCacheOnWriteMiss) {
    CacheConfig cfg(128, 2);
    Cache cache(cfg, 16, PolicyKind::LRU, WritePolicy::WriteBack, AllocPolicy::NoWriteAllocate);

    bool evicted_dirty, wrote_to_memory;

    // Write miss: with no-write-allocate, nothing gets installed.
    EXPECT_FALSE(cache.access(0x00, true, evicted_dirty, wrote_to_memory));
    EXPECT_TRUE(wrote_to_memory);
    EXPECT_FALSE(evicted_dirty);  // nothing was evicted -- nothing was ever installed

    // Prove it really wasn't installed: same address again is STILL a miss.
    EXPECT_FALSE(cache.access(0x00, true, evicted_dirty, wrote_to_memory));
}
