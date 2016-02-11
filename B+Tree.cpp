#include "B+Tree.hpp"

int main() {
    storage::bptree<int> test(4);
    test.insert(storage::key("t2"), 3);
    test.insert(storage::key("t3"), 6);
    test.insert(storage::key("t1"), 6);
    test.insert(storage::key("t4"), 6);

    test.insert(storage::key("t5"), 6);
    test.insert(storage::key("t6"), 6);
    test.insert(storage::key("t7"), 6);
    test.insert(storage::key("t8"), 6);
    test.insert(storage::key("t9"), 6);
    test.insert(storage::key("t10"), 6);
    test.insert(storage::key("t11"), 6);
    test.insert(storage::key("t12"), 6);
    test.insert(storage::key("t13"), 6);
    test.insert(storage::key("t14"), 6);

    test.remove(storage::key("t2"));
    test.remove(storage::key("t3"));
    test.remove(storage::key("t5"));
    //auto obj1 = test.search(storage::key("t12"));
    //auto obj2 = test.search(storage::key("t5"));
    //std::cout << obj1->children.size() << " | " << obj2->children.size() << std::endl;
    //std::cout << obj1->children[0].k << " | " << obj2->children[0].k << std::endl;
}
