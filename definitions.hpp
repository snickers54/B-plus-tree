#ifndef _DEFINITIONS_HPP_
#define _DEFINITIONS_HPP_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include <iostream>
#include <memory>
#include <vector>
#include <experimental/optional>
#include <utility>

namespace std {
    #ifndef optional
        template <typename T>
        using optional = std::experimental::optional<T>;
    #endif
}

namespace storage {

struct key {
    std::string k;

    key() = default;
    ~key() = default;
    key(const char *str) : k(std::string(str)) {}
    key(std::string str) : k(std::move(str)) {}
    const bool operator <(const key& rhs) const {
       return this->k < rhs.k;
    }
    const bool operator <=(const key& rhs) const {
        return this->k <= rhs.k;
    }
    const bool operator >(const key& rhs) const {
        return this->k > rhs.k;
    }
    const bool operator >=(const key& rhs) const {
        return this->k >= rhs.k;
    }
    const bool operator ==(const key& rhs) const {
        return this->k == rhs.k;
    }
    friend void swap(key& a, key& b) {
      std::swap(a.k, b.k);
    }
};

std::ostream& operator<<(std::ostream& os, const key& obj) {
    os << obj.k;
    return os;
}

template <typename Value>
struct node;

template <typename Value>
struct index {
    key k;
    std::shared_ptr<node<Value>> more;
    std::shared_ptr<node<Value>> less;
    std::optional<Value> value;
    index(const index &) = default;
    index(index &&o) noexcept : k(std::move(o.k)), more(o.more), less(o.less), value(o.value) {}
    index(key k2, std::shared_ptr<node<Value>> c, std::shared_ptr<node<Value>> c2, std::optional<Value> v2) : k(k2), more(c), less(c2), value(v2) {}
    index& operator=(index&& oth) {
        if (this != &oth) {
            this->k = std::move(oth.k);
            this->more = oth.more;
            this->less = oth.less;
            oth.more = nullptr;
            oth.less = nullptr;
            this->value = std::move(oth.value);
        }
        return *this;
    }
    index& operator=(const index& oth) {
        if (this != &oth) {
            this->k = oth.k;
            this->more = oth.more;
            this->less = oth.less;
            this->value = oth.value;
        }
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
