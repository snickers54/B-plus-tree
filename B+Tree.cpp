#include "B+Tree.hpp"

namespace storage_struct {

    template <typename Value>
    bool bptree::insert(key k, Value value) {
        if (this->root.size() == 0) {
            node n = node{};
            index i = index{k, value};
            n.children.push_back(i);
            this->root.push_back(n);
            std::cout << this->root[0].children[0].k. << std::end;
            return true;
        }
        return false;
    }
}


int main() {
    storage_struct::bptree<int> test(3, 2);
    test.insert(storage_struct::key{"t1"}, 3);
}
