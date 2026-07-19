#include <gtest/gtest.h>
#include "cachesim/cache.hpp"

using namespace cachesim;

TEST(Cache, HitsMissesAndEviction) {
    CacheConfig cfg(128, 2);  // 128 bytes, 2-way
    Cache cache(cfg, 16, PolicyKind::LRU);  // block_size=16 -> 4 sets

    bool evicted_dirty;

    // 1. First access to a fresh cache: always a miss.
    EXPECT_FALSE(cache.access(0x00, true, evicted_dirty));
    EXPECT_FALSE(evicted_dirty);

    // 2. Same address again: hit.
    EXPECT_TRUE(cache.access(0x00, false, evicted_dirty));

    // 3. Different tag, same set, other way still free: miss, no eviction.
    EXPECT_FALSE(cache.access(0x40, false, evicted_dirty));
    EXPECT_FALSE(evicted_dirty);

    // 4. Still there: hit.
    EXPECT_TRUE(cache.access(0x00, false, evicted_dirty));

    // 5. Set is full -> this miss forces an eviction of 0x40's line (not dirty).
    EXPECT_FALSE(cache.access(0x80, true, evicted_dirty));
    EXPECT_FALSE(evicted_dirty);

    // 6. Another miss -> evicts the 0x00 line, which WAS written in step 1.
    EXPECT_FALSE(cache.access(0xC0, false, evicted_dirty));
    EXPECT_TRUE(evicted_dirty);
}