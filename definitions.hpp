#ifndef _DEFINITIONS_HPP_
#define _DEFINITIONS_HPP_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <memory>

namespace bptree {

    struct neighbor {
        uintptr_t parent;
        uintptr_t prev;
        uintptr_t next;
    };
    struct key {
        char k[42];

        key(const char *str = "")
        {
            strncpy(k, str, sizeof(k));
        }
    };
    template<typename T>
    struct record {
        key k;
        T value;
    };


    struct index {
        key k;
        uintptr_t child;
        index() {
            child = 0;
        }
    };

    struct node {
        neighbor ptrs;
        size_t space_left;
        std::unique_ptr<index[]> children;

        node(const size_t nb_children = 4) : children(std::make_unique<index[]>(nb_children)), space_left(nb_children) {}
    };

    template<typename T>
    struct leaf {
        neighbor ptrs;
        size_t space_left;
        std::unique_ptr<record<T>[]> row;

        leaf(const size_t nb_children = 4) : row(std::make_unique<record<T>[]>(nb_children)), space_left(nb_children) {}
    };

}
#endif
