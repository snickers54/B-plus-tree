#ifndef _BPTREE_HPP_
#define _BPTREE_HPP_

#include "definitions.hpp"
#include <algorithm>

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
            std::cout << "-------------> INSERTING " << k.k << std::endl;
            // we assume our b+tree is empty
            std::shared_ptr<node<Value>> page = this->root;
            // we check it's not empty
            if (this->root->children.size() > 0) {
                // then we are looking for the corresponding node to insert into
                // std::cout << "BTREE IS NOT EMPTY" << std::endl;
                page = this->search(k);
            }
            // std::cout << "PAGE SIZE : " << page->children.size() << std::endl;
            // here, for simplicity I add the node before spliting, avoiding to check if I have to insert on this node or the new one..
            page->children.emplace_back(k, nullptr, nullptr, value);
            this->sort(page);
            if (page->children.size() > this->fanout) {
                //  std::cout << "WE HAVE TO SPLIT BECAUSE OUR NODE IS FULL" << std::endl;
                this->split(page);
            }
            this->print_tree(this->root);
            return false;
        }

        void print_tree(std::shared_ptr<node<Value>> root) {
            this->print_tree(root, 0);
        }
    private:
        const size_t fanout;
        std::shared_ptr<node<Value>> root;

        void print_tree(std::shared_ptr<node<Value>> page, size_t level) {
            if (page == nullptr) return ;
            std::cout << " <<<<<<< LEVEL " << level << " >>>>>>>" << std::endl;
            // we print all indexes into this node
            std::shared_ptr<node<Value>> tmp = page;
            while (tmp->prev != nullptr) {tmp = tmp->prev;}
            while (tmp) {
                print_node(tmp);
                tmp = tmp->next;
            }
            this->print_tree((page->children[0].less) ? page->children[0].less : page->children[0].more, level + 1);
        }

        void print_node(std::shared_ptr<node<Value>> root) {
            for (auto& v : root->children) {
                std::cout << "[" << v.k << "](" << (*v.value) << ") ";
            }
            std::cout << std::endl;
        }

        void split(std::shared_ptr<node<Value>> page) {
            std::shared_ptr<node<Value>> tmp = std::make_shared<node<Value>>();
            // here you could think I should assign to counter this->fanout, BUT because I added the index before spliting, wich means my vector at this moment is of size this->fanout + 1
            // So by assigning the size of the vector, I'm sure, it's gonna split the vector theoric max size by 2.
            size_t counter = page->children.size() - 1;
            while (counter >= (this->fanout / 2)) {
                std::cout << "OUR COUNTER IS AT : " << counter << " : " << page->children[counter].k << std::endl;
                tmp->children.push_back(std::move(page->children[counter--]));
                page->children.pop_back();
            }
            // update pointers to prev / next
            // std::cout << "POINTERS ARE SET HERE " << std::endl;
            tmp->parent = page->parent;
            tmp->prev = page;
            tmp->next = page->next;
            page->next = tmp;
            if (tmp->next != nullptr) {
                tmp->next->prev = tmp;
            }
            // std::cout << "STARTING SORTING OBJECTS" << std::endl;
            this->sort(tmp);
             std::cout << "STARTING INSERTING PARENTS " << page->parent << " " << tmp << std::endl;
            this->insert_parent(page->parent, page);
        }

        void insert_parent(std::shared_ptr<node<Value>> parent, std::shared_ptr<node<Value>> page) {
            // no parent exists because we are on the root, which means we have to create a new upper root
            if (parent == nullptr) {
                // std::cout << "parent is null, we are at root level .. " << std::endl;
                std::shared_ptr<node<Value>> new_root = std::make_shared<node<Value>>();
                // std::cout << "STARTING ADDING INDEX TO THE NEW ROOT POINTER " << page->next << " | " << page << std::endl;
                new_root->children.emplace_back(page->next->children[0].k, page->next, page, false);
                // std::cout << "STARTING PROMOTING PARENTS" << std::endl;
                this->promote_parent(this->root, new_root);
                // std::cout << "ASSIGNING NEW ROOT" << std::endl;
                this->root = new_root;
            } else {
                // here we try to insert first node of our newer split page to the parent
                // if the parent is full we have to split, and will do it until we don't have to split any new parent ..
                if (parent->children.size() >= this->fanout) {
                    std::cout << "WE HAVE TO SPLIT OUR PARENT" << std::endl;
                    this->split(parent);
                }
                parent->children.emplace_back(page->next->children[0].k, page->next, page, false);
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
        void sort(std::shared_ptr<node<Value>> toto) {
            std::sort(toto->children.begin(), toto->children.end(),
                [](const index<Value> &a, const index<Value> &b) -> bool {
                    return a.k.k < b.k.k;
                }
            );
        }

        std::shared_ptr<node<Value>> tree_search(key k, std::shared_ptr<node<Value>> n) {
            // if it's a leaf ..
            if (n->children[0].more == nullptr && n->children[0].less == nullptr) {
                // std::cout << "THIS IS A LEAF WE JUST RETURN IT" << std::endl;
                return n;
            }
            std::shared_ptr<node<Value>> tmp = nullptr;
            // std::cout << "GOING THROUGH CHILDRENS AND CHECKING " << k << std::endl;
            for (auto& v : n->children) {
                //std::cout << v.k << std::endl;
                if (k.k > v.k.k) {
                    // std::cout << "more !" << std::endl;
                    tmp = v.more;
                    continue;
                }
                if (k.k <= v.k.k) {
                    // std::cout << "less !" << std::endl;
                    tmp = v.less;
                    break;
                }
            }
            // std::cout << "CALLING MYSELF WITH : " << tmp << std::endl;
            return this->tree_search(k, tmp);
        }
    };
}
#endif
