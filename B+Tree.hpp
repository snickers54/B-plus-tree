#ifndef _BPTREE_HPP_
#define _BPTREE_HPP_

#include "definitions.hpp"
#include <algorithm>

namespace storage {

    template <typename Value>
    class bptree {
    private:
        class iterator : public std::iterator<std::input_iterator_tag, std::shared_ptr<node<Value>>> {

            public:
                iterator(std::shared_ptr<node<Value>>pointer, size_t pos) : pointer(pointer), pos(pos){}
                bool operator==(const iterator& rhs) {
                    return (this->pointer == rhs.pointer && (this->pos) == (rhs.pos));
                }
                bool operator!=(const iterator& rhs) {
                    return (this->pointer != rhs.pointer || (this->pos) != (rhs.pos));
                }

                void operator++() {
                    ++this->pos;
                    // std::cout << "position " << this->pos << std::endl;
                    if (this->pos >= this->pointer->children.size()) {
                        // std::cout << "passing from " << this->pointer << " to " << this->pointer->next << std::endl;
                        this->pointer = this->pointer->next;
                        this->pos = 0;
                    }
                }

                std::optional<index<Value>> operator*() {
                    std::optional<index<Value>> option;
                    if (this->pointer) {
                        return std::experimental::make_optional(this->pointer->children[this->pos]);
                    }
                    return option;
                }

                ~iterator(){}

            private:
                size_t pos = 0;
                std::shared_ptr<node<Value>> pointer;
        };
    public:

        /* BPTREE class */
        bptree(size_t fanout) : fanout(fanout), root(std::make_shared<node<Value>>()){}
        ~bptree(){};

        iterator search(key k)  {
            auto node = this->tree_search(k, this->root);
            size_t pos = 0;
            for (auto& v : node->children) {
                if (v.k == k) {
                    return iterator(node, pos);
                }
                pos++;
            }
            return iterator(nullptr, 0);
        }

        std::pair<iterator, iterator> search_range(key k1, key k2) {
            if (k2 < k1) {
                std::swap(k1, k2);
            }
            auto it_start = this->search(k1);
            auto it_end = this->search(k2);
            if ((*it_end)) {
                ++it_end;
            } else {
                it_end = iterator(nullptr, 0);
            }
            return std::make_pair(it_start, it_end);
        }

        iterator begin() {
            return iterator(this->first(this->root), 0);
        }

        iterator end() {
            return iterator(nullptr, 0);
        }

        void insert(key k, Value value) {
            std::cout << "-------------> INSERTING " << k.k << std::endl;
            // we assume our b+tree is empty
            auto page = this->root;
            // we check it's not empty
            if (this->root->children.size() > 0) {
                // then we are looking for the corresponding node to insert into
                // std::cout << "BTREE IS NOT EMPTY" << std::endl;
                page = this->search_node(k);
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
            this->nb_values++;
        }

        void print_tree(std::shared_ptr<node<Value>> root) {
            this->print_tree(root, 0);
        }

        bool remove(key k) {
            std::cout << "-------------> REMOVING " << k.k << std::endl;
            if (this->root->children.size() <= 0) {return false;}
            auto page = this->search_node(k);
            if (page == nullptr) {return false;}
            std::cout << "FOUND NODE " << page << " | " << page->children.size() << " | " << page->prev << " | " << (this->fanout / 2) << std::endl;
            if ((page->children.size() - 1) < (this->fanout / 2) && page->prev) {
                std::cout << "SIZE OF LEAF IS < this->fanout / 2 and prev = " << page->prev << std::endl;
                // we borrow from a sibling
                if (page->prev->children.size() > (this->fanout / 2)) {
                    page->children.push_back(std::move(page->prev->children[page->prev->children.size() - 1]));
                    page->prev->children.pop_back();
                } else {
                    // we just merge with a sibling ..
                    auto tmpkey = page->children[0].k;
                    for (auto& v : page->children) {
                        if (v.k == k) continue;
                        page->prev->children.push_back(std::move(v));

                    }
                    // before messing with ptrs, we should change index ptrs which were pointing to our node..
                    // because experimental::optional implement operator bool()
                    // so here it means, if you are not a leaf ..
                    std::cout << "BEFORE JUMP NODE IN REMOVE BY KEY (" << tmpkey << ") "<< page->parent << std::endl;
                    if (page->parent != nullptr) {
                        auto counter = this->find_position_index(tmpkey, page->parent);
                        auto parent = page->parent;
                        this->print_node(parent);
                        std::cout << "Found " << k << " at position : " << *counter << std::endl;
                        if (*counter + 1 > parent->children.size() - 1 && parent->next) {
                            std::cout << "Is in next node" << std::endl;
                            auto next = parent->next;
                            std::cout << next->children[0].k << ".less is goin to " << parent->children[*counter].less << std::endl;
                            next->children[0].less = parent->children[*counter].less;
                        } else {
                            std::cout << parent->children[*counter + 1].k << ".less is goint to " << parent->children[*counter].less << std::endl;
                            parent->children[*counter + 1].less = parent->children[*counter].less;
                        }
                    }
                    this->remove_by_key(tmpkey, page->parent);
                    this->jump_node(page);
                }
                this->sort(page);
            }
            this->remove_by_key(k, page);
            this->print_tree(this->root);
            this->nb_values--;
            return true;
        }
        void print_node(std::shared_ptr<node<Value>> root) {
            if (root == nullptr) return ;
            std::cout << "Parent : " << root->parent << " me : " << root << "NEXT : " << root->next << " PREV : " << root->prev ;
            for (auto& v : root->children) {
                std::cout << " | [(" << v.less << ") " << v.k << " (" << v.more << ")] ";
            }
            std::cout << std::endl;
        }

        bool update(key k, Value value) {
            auto index = this->search_index(k);
            if (index == nullptr) return false;
            std::optional<Value> tmp = value;
            index->value = std::move(tmp);
            return true;
        }
        // return the number of values currently in the tree. Nothing to do with the height
        size_t size() {
            return this->nb_values;
        }
        // return the height of your bptree
        size_t height() {
            return this->height(this->root);
        }

    private:
        const size_t fanout;
        std::shared_ptr<node<Value>> root;
        size_t nb_values = 0;
        std::shared_ptr<node<Value>> search_node(key k) {
            return this->tree_search(k, this->root);
        }

        size_t height(std::shared_ptr<node<Value>> root) {
            if (root == nullptr) {
                return 0;
            }
            return 1 + this->height(root->children[0].less);
        }

        void print_tree(std::shared_ptr<node<Value>> page, size_t level) {
            if (page == nullptr) return ;
            std::cout << " <<<<<<< LEVEL " << level << " >>>>>>>" << std::endl;
            // we print all indexes into this node
            auto tmp = page;
            while (tmp->prev != nullptr) {tmp = tmp->prev;}
            while (tmp) {
                print_node(tmp);
                tmp = tmp->next;
            }
            this->print_tree((page->children[0].less) ? page->children[0].less : page->children[0].more, level + 1);
        }

        void remove_by_key(key k, std::shared_ptr<node<Value>> page) {
            std::cout << "REMOVE BY KEY : " << k << std::endl;
            if (page == nullptr) {return;}
            for (auto it = page->children.begin(); it != page->children.end(); ++it) {
                std::cout << "CHECKING ON  " << (*it).k << std::endl;
                if ((*it).k.k == k.k) {
                    std::cout << "ERASING " << k << std::endl;
                    page->children.erase(it);
                    break;
                }
            }
            // this node is empty.. we fix pointers
            std::cout << "SIZE OF PAGE->CHILDREN IS " << page->children.size() << std::endl;
            if (page->children.size() == 0) {
                this->jump_node(page);
                // in that case we erase for good the key in parents
                return this->remove_by_key(k, page->parent);
            }
            // else, we just assign the new key ..
            this->assign_parent(k, page);
        }

        void assign_parent(key k, std::shared_ptr<node<Value>> page) {
            if (page->parent) {
                std::cout << "ASSIGN NEW KEY TO PARENT .. " << std::endl;
                for (auto& v : page->parent->children) {
                    std::cout << v.k << std::endl;
                    if (v.k == k) {
                        std::cout << " FOUND FOUND " << std::endl;
                        v.k.k = page->children[0].k.k;
                        break;
                    }
                }
                this->assign_parent(k, page->parent);
            }
        }

        void split(std::shared_ptr<node<Value>> page) {
            auto tmp = std::make_shared<node<Value>>();
            // here you could think I should assign to counter this->fanout, BUT because I added the index before spliting, wich means my vector at this moment is of size this->fanout + 1
            // So by assigning the size of the vector, I'm sure, it's gonna split the vector theoric max size by 2.
            size_t counter = page->children.size() - 1;
            while (counter >= (this->fanout / 2)) {
                std::cout << "OUR COUNTER IS AT : " << counter << " : " << page->children[counter].k << std::endl;
                if (page->children[counter].more != nullptr) {
                    page->children[counter].more->parent = tmp;
                }
                tmp->children.push_back(std::move(page->children[counter--]));
                page->children.pop_back();
            }
            // update pointers to prev / next
            std::cout << "POINTERS ARE SET HERE " << std::endl;
            tmp->parent = page->parent;
            tmp->prev = page;
            tmp->next = page->next;
            page->next = tmp;
            if (tmp->next != nullptr) {
                tmp->next->prev = tmp;
            }
            std::cout << "STARTING SORTING OBJECTS" << std::endl;
            this->sort(tmp);
             std::cout << "STARTING INSERTING PARENTS " << page->parent << " " << tmp << std::endl;
            this->insert_parent(page->parent, page);
        }

        void insert_parent(std::shared_ptr<node<Value>> parent, std::shared_ptr<node<Value>> page) {
            using namespace std::experimental;

            // no parent exists because we are on the root, which means we have to create a new upper root
            if (parent == nullptr) {
                // std::cout << "parent is null, we are at root level .. " << std::endl;
                auto new_root = std::make_shared<node<Value>>();
                // std::cout << "STARTING ADDING INDEX TO THE NEW ROOT POINTER " << page->next << " | " << page << std::endl;
                new_root->children.emplace_back(page->next->children[0].k, page->next, page, nullopt);
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
                std::cout << "PARENT WAS NOT NULL " << std::endl;
                parent->children.emplace_back(page->next->children[0].k, page->next, page, nullopt);
                this->sort(parent);
                // here we have to find the index just before the one we inserted, to change his less / more pointers
                std::cout << " LOOKING FOR INDEX OF " << page->next->children[0].k << std::endl;
                auto counter = this->find_position_index(page->next->children[0].k, parent);
                // experimental::optional is implementing operator bool() ..
                if (counter) {
                    std::cout << (*counter) << " IS THE POSITION" << std::endl;
                    if ((*counter) + 1 < parent->children.size()) {
                        parent->children[(*counter) + 1].less = page->next;
                    }
                }
            }
            this->sort(this->root);
        }

        std::optional<size_t> find_position_index(key k, std::shared_ptr<node<Value>> page) {
            size_t counter = 0;
            std::optional<size_t> result;
            std::cout << page->children.size() << std::endl;
            while (counter < page->children.size()) {
                std::cout << page->children[counter].k << std::endl;
                if (page->children[counter].k.k == k.k) {
                    result = counter;
                    std::cout << "FOUND " << k << " : " << counter << std::endl;
                    break;
                }
                counter++;
            }
            return result;
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

        // when we delete one node or leaf, we want their direct siblings to just jump it in term of prev / next ptrs ..
        void jump_node(std::shared_ptr<node<Value>> page) {

            if (page->prev) {
                page->prev->next = page->next;
            }
            page->parent = nullptr;
            if (page->next) {
                page->next->prev = page->prev;
            }
        }
        index<Value> *search_index(key k)  {
            auto node = this->tree_search(k, this->root);
            size_t pos = 0;
            for (auto& v : node->children) {
                if (v.k == k) {
                    return &v;
                }
                pos++;
            }
            return nullptr;
        }
        std::shared_ptr<node<Value>> first(std::shared_ptr<node<Value>> ptr) {
            if (ptr->children[0].less == nullptr) {
                return ptr;
            }
            return this->first(ptr->children[0].less);
        }
        std::shared_ptr<node<Value>> tree_search(key k, std::shared_ptr<node<Value>> n) {
            // if it's a leaf ..
            if (n == nullptr || (n->children[0].more == nullptr && n->children[0].less == nullptr)) {
                // std::cout << "THIS IS A LEAF WE JUST RETURN IT" << std::endl;
                return n;
            }
            std::shared_ptr<node<Value>> tmp = nullptr;
            // std::cout << "GOING THROUGH CHILDRENS AND CHECKING " << k << std::endl;
            for (auto& v : n->children) {
                //std::cout << v.k << std::endl;
                // this->print_node(v.less);
                // this->print_node(v.more);
                if (k >= v.k) {
                    // std::cout << "more ! : " << v.k << std::endl;
                    tmp = v.more;
                    continue;
                }
                if (k < v.k) {
                    // std::cout << "less ! : " << v.k << std::endl;
                    tmp = v.less;
                    break;
                }
            }
            // std::cout << "CALLING MYSELF WITH : " << tmp << std::endl;
            return this->tree_search(k, tmp);
        }
    };
    /* END BPTREE Class*/

}
#endif
