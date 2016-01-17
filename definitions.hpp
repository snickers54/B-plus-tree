#ifndef _DEFINITIONS_HPP_
#define _DEFINITIONS_HPP_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <memory>
#include <vector>

namespace storage_struct {

struct key {
    std::string k;

    key() = default;
    ~key() = default;
    explicit key(std::string str) : k(std::move(str)) {}
};
template <typename Value>
struct record {
    key k;
    Value value;
    record(record &&o) noexcept : k(std::move(o.k)), value(std::move(o.value)) {}
};

struct index {
    key k;
    uintptr_t child;
    index(index &&i) noexcept : k(std::move(i.k)) {}
};

template <typename T>
struct element {
    uintptr_t parent;
    uintptr_t prev;
    uintptr_t next;
    std::vector<T> children;
};

template <typename Value>
using leaf = element<record<Value>>;
using node = element<index>;

}
#endif
