#ifndef _BPTREE_HPP_
#define _BPTREE_HPP_


#include "definitions.hpp"

namespace bptree {

    class BplusTree {
    public:
        BplusTree(size_t height = 3, size_t order = 4);
        ~BplusTree();
    private:
        const size_t height;
        const size_t order;
        std::unique_ptr<node> root;
    };

}
#endif
