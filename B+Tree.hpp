#ifndef _BPTREE_HPP_
#define _BPTREE_HPP_

#include "definitions.hpp"

namespace storage_struct {

    template <typename Value>
    class bptree {
    public:
        bptree(size_t height, size_t order) : height(height), order(order){}
        ~bptree(){};
        bool insert(key, Value);
    private:
        const size_t height;
        const size_t order;
        std::vector<node> root;
    };

}
#endif
