#ifndef _BPTREE_HPP_
#define _BPTREE_HPP_

#include "definitions.hpp"

namespace storage {

    template <typename Value>
    class bptree {
    public:
        bptree(size_t fanout) : fanout(fanout), root(std::make_shared<node<Value>>()){}
        ~bptree(){};
        std::shared_ptr<node<Value>> search(key k) {
            return this->tree_search(k, this->root);
        }
        bool insert(key k, Value value) {
            // we assume our b+tree is empty
            std::shared_ptr<node<Value>> page = this->root;
            // we check it's not empty
            if (this->root->children.size() != 0) {
                // then we are looking for the corresponding node to insert into
                page = this->search(k);
                std::cerr << "PAGE IS NOT ROOT" << std::endl;
            }
            std::cerr << "PAGE SIZE : " << page->children.size() << std::endl;
            // if there is enough place in this node, we just have to push and sort in our vector
            if (page->children.size() >= this->fanout) {
                std::cerr << "WE HAVE TO SPLIT BECAUSE OUR NODE IS FULL" << std::endl;
                this->split(page);
            }
            std::cerr << "FINALLY ADDING OUR VALUE" << std::endl;
            page->children.emplace(page->children.end(), k, nullptr, nullptr, value);
            std::cerr << "SORTING OUR NODE'S INDEX" << std::endl;
            this->sort(page);
            std::cerr << "IT'S INSERTED GG" << std::endl;
            return false;
        }
    private:
        const size_t fanout;
        std::shared_ptr<node<Value>> root;

        void split(std::shared_ptr<node<Value>> page) {
            std::shared_ptr<node<Value>> tmp = std::make_shared<node<Value>>();
            size_t counter = this->fanout;
            while (counter > (this->fanout / 2)) {
                std::cerr << "OUR COUNTER IS AT : " << counter << std::endl;
                tmp->children.push_back(std::move(page->children[counter--]));
                page->children.pop_back();
            }
            // update pointers to prev / next
            std::cerr << "POINTERS ARE SET HERE " << std::endl;
            tmp->parent = page->parent;
            tmp->prev = page;
            tmp->next = page->next;
            page->next = tmp;
            if (tmp->next != nullptr) {
                tmp->next->prev = tmp;
            }
            std::cerr << "STARTING SORTING OBJECTS" << std::endl;
            this->sort(tmp);
            std::cerr << "STARTING INSERTING PARENTS " << page->parent << " " << tmp << std::endl;
            this->insert_parent(page->parent, tmp);
        }

        void insert_parent(std::shared_ptr<node<Value>> parent, std::shared_ptr<node<Value>> page) {
            // no parent exists because we are on the root, which means we have to create a new upper root
            if (parent == nullptr) {
                std::cerr << "parent is null, we are at root level .. " << std::endl;
                std::shared_ptr<node<Value>> new_root = std::make_shared<node<Value>>();
                std::cerr << "STARTING ADDING INDEX TO THE NEW ROOT POINTER" << std::endl;
                new_root->children.emplace(new_root->children.end(), page->children[0].k, page->next, page, false);
                std::cerr << "STARTING PROMOTING PARENTS" << std::endl;
                this->promote_parent(this->root, new_root);
                std::cerr << "ASSIGNING NEW ROOT" << std::endl;
                this->root = new_root;
            } else {
                // here we try to insert first node of our newer split page to the parent
                // if the parent is full we have to split, and will do it until we don't have to split any new parent ..
                if (parent->children.size() >= this->fanout) {
                    this->split(parent);
                }
                parent->children.emplace(page->children.end(), page->children[0].k, page->next, page, false);
            }
        }
        void promote_parent(std::shared_ptr<node<Value>> starting, std::shared_ptr<node<Value>> parent) {
            auto tmp = starting;
            while (tmp) {
                tmp->parent = parent;
                tmp = tmp->prev;
            }

            tmp = starting;
            while (tmp) {
                tmp->parent = parent;
                tmp = tmp->next;
            }
        }
        void sort(std::shared_ptr<node<Value>> page) {
            std::sort(page->children.begin(), page->children.end(),
                [](const auto &a, const auto &b) -> bool {
                    return a.k.k > b.k.k;
                }
            );
        }

        std::shared_ptr<node<Value>> tree_search(key k, std::shared_ptr<node<Value>> n) {
            // if it's a leaf ..
            if (n->children[0].more == nullptr && n->children[0].less == nullptr) {
                return n;
            }
            std::shared_ptr<node<Value>> tmp = nullptr;
            for (auto& v : n->children) {
                if (k.k > v.k.k) {
                    tmp = v.more;
                    continue;
                }
                if (k.k <= v.k.k) {
                    tmp = v.less;
                    break;
                }
            }
            return this->tree_search(k, tmp);
        }
    };
}
#endif
