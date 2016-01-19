#include "B+Tree.hpp"

int main() {
    storage::bptree<int> test(4);
    test.insert(storage::key("t1"), 3);
    test.insert(storage::key("t2"), 6);
    test.insert(storage::key("t3"), 6);
    test.insert(storage::key("t4"), 6);
    test.insert(storage::key("t5"), 6);
    test.insert(storage::key("t6"), 6);

    auto obj1 = test.search(storage::key("t1"));
    auto obj2 = test.search(storage::key("t5"));
    std::cout << obj1->children.size() << " | " << obj2->children.size() << std::endl;
    std::cout << obj1->children[0].k.k << " | " << obj2->children[0].k.k << std::endl;
}
