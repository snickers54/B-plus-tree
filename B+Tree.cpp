#include "B+Tree.hpp"

int main() {
    storage::bptree<std::string> test(4);
    test.insert(storage::key("t2"), std::string("t2"));
    test.insert(storage::key("t3"), std::string("t3"));
    test.insert(storage::key("t1"), std::string("t1"));
    test.insert(storage::key("t4"), std::string("t4"));

    test.insert(storage::key("t5"), std::string("t5"));
    test.insert(storage::key("t6"), std::string("t6"));
    test.insert(storage::key("t7"), std::string("t7"));
    test.insert(storage::key("t8"), std::string("t8"));
    test.insert(storage::key("t9"), std::string("t9"));
    test.insert(storage::key("t10"), std::string("t10"));
    test.insert(storage::key("t11"), std::string("t11"));
    test.insert(storage::key("t12"), std::string("t12"));
    test.insert(storage::key("t13"), std::string("t13"));
    test.insert(storage::key("t14"), std::string("t14"));

    // test.remove(storage::key("t2"));
    // test.remove(storage::key("t3"));
    // test.remove(storage::key("t5"));
    auto obj1 = test.search(storage::key("t12"));
    auto obj2 = test.search(storage::key("t5"));
    std::cout << *(**obj1).value << std::endl;

    test.update(storage::key("t12"), "42");
    std::cout << *(**obj1).value << std::endl;

    std::cout << "-----------" << std::endl;
    auto p = test.search_range("t1", "t5");
    for (auto it = p.first; it != p.second; ++it) {
        std::cout << **it << std::endl;
    }

    std::cout << "-----------" << std::endl;
    for (const auto &v : test) {
        std::cout << *v << std::endl;
    }

    std::cout << "-----------" << std::endl;
    std::cout << test.size() << " | " << test.height() << std::endl;

}
