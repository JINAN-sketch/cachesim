#ifndef CACHESIM_CACHE_LINE_HPP
#define CACHESIM_CACHE_LINE_HPP

#include <cstdint>

namespace cachesim{

struct CacheLine{
    uint64_t    tag = 0;
    bool        valid = false;
    bool        dirty = false;        
};

}
#endif
