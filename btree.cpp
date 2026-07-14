#include "btree.h"
#include <iostream>
#include <stdexcept>

BTree::Node::Node(bool is_leaf)
    : leaf(is_leaf) {
}

BTree::BTree(int min_degree)
    : root_(new Node(true)), min_degree_(min_degree) {
    if (min_degree < 2) {
        throw std::runtime_error("BTree min_degree must be at least 2");
    }
}

BTree::~BTree() {
    destroy(root_);
}

bool BTree::contains(int key) const {
    return search(root_, key);
}

void BTree::insert(int key) {
    if (contains(key)) {
        return;
    }

    if (is_full(root_)) {
        Node *new_root = new Node(false);
        new_root->children.push_back(root_);

        split_child(new_root, 0);

        root_ = new_root;
    }

    insert_non_full(root_, key);
}

void BTree::print() const {
    print_node(root_, 0);
}

int BTree::max_keys() const {
    return 2 * min_degree_ - 1;
}

bool BTree::is_full(Node *node) const {
    return node->keys.size() == max_keys();
}

bool BTree::search(Node *node, int key) const {
    int i = 0;

    while (i < node->keys.size() && key > node->keys[i]) {
        ++i;
    }

    if (i < node->keys.size() && key == node->keys[i]) {
        return true;
    }

    if (node->leaf) {
        return false;
    }

    return search(node->children[i], key);
}


bool is_full(Node *node) const {
    return static_cast<int>(node->keys.size()) == max_keys();
}

void destroy(Node *node) {
    if (node == nullptr) {
        return;
    }

    for (Node *child: node->children) {
        destroy(child);
    }

    delete node;
}

void insert_non_full(Node *node, int key) {
    int i = static_cast<int>(node->keys.size()) - 1;

    if (node->leaf) {
        node->keys.push_back(0);
        while (i >= 0 && key < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            --i;
        }
        node->keys[i + 1] = key;
        return;
    }
    while (i >= 0 && key < node->keys[i]) {
        --i;
    }
    ++i;
    if (is_full(node->children[i])) {
        split_child(node, i);
        if (key > node->keys[i]) {
            ++i;
        }
    }

    insert_non_full(node->children[i], key);
}

void BTree::split_child(Node *parent, int child_index) {
    Node *left = parent->children[child_index];
    Node *right = new Node(left->leaf);

    int middle_index = min_degree_ - 1;
    int middle_key = left->keys[middle_index];
    for (int j = middle_index + 1; j < static_cast<int>(left->keys.size()); ++j) {
        right->keys.push_back(left->keys[j]);
    }
    left->keys.resize(static_cast<std::size_t>(middle_index));
    if (!left->leaf) {
        for (int j = min_degree_; j < static_cast<int>(left->children.size()); ++j) {
            right->children.push_back(left->children[j]);
        }

        left->children.resize(static_cast<std::size_t>(min_degree_));
    }

    parent->keys.insert(parent->keys.begin() + child_index, middle_key);
    parent->children.insert(parent->children.begin() + child_index + 1, right);
}

void BTree::print_node(Node *node, int depth) const {
    for (int i = 0; i < depth; ++i) {
        std::cout << "  ";
    }
    std::cout << "[";
    for (std::size_t i = 0; i < node->keys.size(); ++i) {
        std::cout << node->keys[i];

        if (i + 1 < node->keys.size()) {
            std::cout << " ";
        }
    }

    std::cout << "]\n";

    for (Node *child: node->children) {
        print_node(child, depth + 1);
    }
}
