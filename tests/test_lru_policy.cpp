#include <gtest/gtest.h>
#include "cachesim/policies/lru_policy.hpp"

using cachesim::LruPolicy;

TEST(LruPolicy, EvictsLeastRecentlyUsed) {
    LruPolicy lru(4);  // ways 0,1,2,3

    // Fill all 4 ways in order: 0,1,2,3 (on_insert = "just became MRU").
    lru.on_insert(0);
    lru.on_insert(1);
    lru.on_insert(2);
    lru.on_insert(3);
    // Order now (MRU->LRU): 3,2,1,0. Way 0 is the LRU victim.
    EXPECT_EQ(lru.victim(), 0u);

    // Access way 0 -> it becomes MRU again. Way 1 is now the LRU victim.
    lru.on_access(0);
    EXPECT_EQ(lru.victim(), 1u);

    // Access way 2 -> becomes MRU. LRU is still way 1 (unaffected).
    lru.on_access(2);
    EXPECT_EQ(lru.victim(), 1u);
}

TEST(LruPolicy, Name) {
    LruPolicy lru(4);
    EXPECT_EQ(lru.name(), "LRU");
}