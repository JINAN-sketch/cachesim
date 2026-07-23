#include <gtest/gtest.h>
#include "cachesim/address.hpp"

using cachesim::decompose_address;

// Hand-computed table, block_size=16 (offset_bits=4), num_sets=4 (index_bits=2).
// Verified on paper before writing these assertions
TEST(AddressDecomposition, KnownValues) {
    auto r1 = decompose_address(0x00, 16, 4);
    EXPECT_EQ(r1.tag, 0u);
    EXPECT_EQ(r1.index, 0u);

    auto r2 = decompose_address(0x10, 16, 4);
    EXPECT_EQ(r2.tag, 0u);
    EXPECT_EQ(r2.index, 1u);

    auto r3 = decompose_address(0x40, 16, 4);
    EXPECT_EQ(r3.tag, 1u);
    EXPECT_EQ(r3.index, 0u);

    auto r4 = decompose_address(0x54, 16, 4);
    EXPECT_EQ(r4.tag, 1u);
    EXPECT_EQ(r4.index, 1u);
}

TEST(AddressDecomposition, Log2OfPowerOfTwo) {
    EXPECT_EQ(cachesim::log2_of_power_of_two(1), 0u);
    EXPECT_EQ(cachesim::log2_of_power_of_two(2), 1u);
    EXPECT_EQ(cachesim::log2_of_power_of_two(16), 4u);
    EXPECT_EQ(cachesim::log2_of_power_of_two(256), 8u);
}
TEST(AddressDecomposition, RecomposeIsInverseOfDecompose) {
    // Round-trip property: decomposing then recomposing should return
    // the original address (with offset bits zeroed, since decompose
    // discards them -- we only test addresses that are already block-aligned).
    uint64_t addr = 0x54;  // block_size=16, num_sets=4 -- same config as KnownValues
    auto d = cachesim::decompose_address(addr, 16, 4);
    uint64_t recomposed = cachesim::recompose_address(d.tag, d.index, 16, 4);
    EXPECT_EQ(recomposed, addr & ~0xFull);  // clear the low 4 offset bits to compare fairly
}