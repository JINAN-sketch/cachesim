#ifndef CACHESIM_LRU_POLICY_HPP
#define CACHESIM_LRU_POLICY_HPP
#include <list>
#include <vector>
#include "cachesim/replacement_policy.hpp"
namespace cachesim{
// LRU: keeps a list of ways, most-recently-used at the front.
// A vector of iterators lets us jump to any way's spot in the list
// instantly, instead of searching for it.
class LruPolicy : public ReplacementPolicy{

public:
    explicit LruPolicy(size_t ways) : iters_(ways){
        // Start with every way "in" the list, least-recent first,
        // so victim() has something valid to return before any access.
        for(size_t w= 0; w < ways;w++){
            order_.push_back(w);
            iters_[w] = std::prev(order_.end());
        }
    }

    size_t victim() const override {
        return order_.back();  // least-recently-used way is at the back... 
    }

    void on_access(size_t way) override {
        touch(way);
    }

    void on_insert(size_t way) override {
        touch(way);
    }

    std::string_view name() const override { return "LRU"; }


private:
    // Move `way` to the front of the list (it's now most-recently-used).
    void touch(size_t way) {
        order_.erase(iters_[way]);
        order_.push_front(way);
        iters_[way] = order_.begin();
    }

    std::list<size_t> order_;                          // MRU front, LRU back
    std::vector<std::list<size_t>::iterator> iters_;   // way -> its list spot
};
}
#endif