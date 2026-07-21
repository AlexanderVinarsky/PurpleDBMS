#ifndef PURPLEDBMS_BTREE_H
#define PURPLEDBMS_BTREE_H

#include <vector>

class BTree {
private:
    struct Node {
        bool leaf;
        std::vector<int> keys;
        std::vector<Node*> children;
        explicit Node(bool is_leaf);
    };
    Node* root_;
    int min_degree_;

public:
    explicit BTree(int min_degree);
    ~BTree();
    BTree(const BTree&) = delete;
    BTree& operator=(const BTree&) = delete;

    bool contains(int key) const;
    void insert(int key);
    void erase(int key);
    void print() const;

private:
    int max_keys() const;
    bool is_full(Node* node) const;
    bool search(Node* node, int key) const;
    void insert_non_full(Node* node, int key);
    void split_child(Node* parent, int child_index);
    void remove_key(Node* node, int key);
    void remove_from_leaf(Node* node, int key_index);
    void remove_from_non_leaf(Node* node, int key_index);
    int get_predecessor(Node* node) const;
    int get_successor(Node* node) const;
    void fill_child(Node* node, int child_index);
    void borrow_from_previous(Node* node, int child_index);
    void borrow_from_next(Node* node, int child_index);
    void merge_children(Node* node, int child_index);
    void print_node(Node* node, int depth) const;
    int find_key_position(Node* node, int key) const;
    void destroy(Node* node);
};

#endif