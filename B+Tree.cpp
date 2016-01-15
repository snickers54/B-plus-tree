
#include "B+Tree.hpp"

namespace bptree {
    BplusTree::BplusTree(size_t height, size_t order) : height(height), order(order), root(std::make_unique<node>(order)){}
    BplusTree::~BplusTree() {}
}


int main(int ac, char **av) {

}
