#include "B+Tree.hpp"

int main() {
    std::vector<std::string> v{"5", "4", "3", "2", "1"};

    std::sort(v.begin(), v.end(), std::greater<std::string>());
    for (auto& n : v) {
        std::cout << n << std::endl;
    }

    storage::bptree<int> test(4);
    test.insert(storage::key("t2"), 3);
    test.insert(storage::key("t3"), 6);
    test.insert(storage::key("t1"), 6);
    test.insert(storage::key("t4"), 6);

    test.insert(storage::key("t5"), 6);
    test.insert(storage::key("t6"), 6);

    auto obj1 = test.search(storage::key("t1"));
    auto obj2 = test.search(storage::key("t5"));
    std::cout << obj1->children.size() << " | " << obj2->children.size() << std::endl;
    std::cout << obj1->children[0].k << " | " << obj2->children[0].k << std::endl;
}
