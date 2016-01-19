#ifndef _DEFINITIONS_HPP_
#define _DEFINITIONS_HPP_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <memory>
#include <vector>
#include <experimental/optional>

namespace storage {

struct key {
    std::string k;

    key() = default;
    ~key() = default;
    explicit key(std::string str) : k(std::move(str)) {}
};
template <typename Value>
struct node;

template <typename Value>
struct index {
    key k;
    std::shared_ptr<node<Value>> more;
    std::shared_ptr<node<Value>> less;
    std::experimental::optional<Value> value;
    index(const index &) = default;
    index(index &&o) noexcept : k(std::move(o.k)), more(o.more), less(o.less), value(o.value) {}
    index(key k2, std::shared_ptr<node<Value>> c, std::shared_ptr<node<Value>> c2, Value v2) : k(k2), more(c), less(c2), value(v2) {}
    index& operator=(index&& other) {
        return *this;
    }
    index& operator=(const index&) {
        return *this;
    }
};

template <typename Value>
struct node {
    std::shared_ptr<node> parent;
    std::shared_ptr<node> prev;
    std::shared_ptr<node> next;

    std::vector<index<Value>> children;
};

}
#endif
