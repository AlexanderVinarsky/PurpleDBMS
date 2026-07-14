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
    void print() const;

private:
    int max_keys() const;
    bool is_full(Node* node) const;
    bool search(Node* node, int key) const;
    void insert_non_full(Node* node, int key);
    void split_child(Node* parent, int child_index);
    void print_node(Node* node, int depth) const;
    void destroy(Node* node);
};

#endif