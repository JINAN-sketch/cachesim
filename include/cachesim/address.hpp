#ifndef CACHESIM_ADDRESS_HPP
#define CACHESIM_ADDRESS_HPP

#include <cstdint>
#include <cstddef>
namespace cachesim{

struct Decomposed{
    uint64_t tag;
    size_t index;
};

//Computes log2(n)
inline unsigned log2_of_power_of_two(uint64_t n) {
    unsigned bits = 0;
    while ((n >>= 1) != 0) {
        ++bits;
    }
    return bits;
}

// Splits addr into {tag, index} given the block size and number of sets.
inline Decomposed decompose_address(uint64_t addr, uint64_t block_size, uint64_t num_sets){
    unsigned offset_bits = log2_of_power_of_two(block_size);
    unsigned index_bits = log2_of_power_of_two(num_sets);

    // Shift off the offset bits, then mask to keep only index_bits worth.
    // (num_sets - 1) works as a mask because num_sets is a power of two:
    // e.g. num_sets=4 -> mask=3=0b011, keeping exactly 2 bits.
    uint64_t index_mask = num_sets - 1;
    uint64_t index = (addr >> offset_bits) & index_mask;

    // Whatever's left after removing offset AND index bits is the tag.
    uint64_t tag = addr >> (offset_bits + index_bits);

    Decomposed result;
    result.tag   = tag;
    result.index = static_cast<size_t>(index);
    return result;
}
inline uint64_t recompose_address(uint64_t tag, size_t index,uint64_t block_size,uint64_t num_sets){
    unsigned offset_bits = log2_of_power_of_two(block_size);
    unsigned index_bits = log2_of_power_of_two(num_sets);

    uint64_t index_part = static_cast<uint64_t>(index) << offset_bits;
    uint64_t tag_part = tag << (offset_bits + index_bits);
    return tag_part | index_part;
}
}
#endif