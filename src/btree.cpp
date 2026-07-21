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

void BTree::erase(int key) {
    remove_key(root_, key);

    if (!root_->leaf && root_->keys.empty()) {
        Node *old_root = root_;
        root_ = root_->children[0];
        delete old_root;
    }
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
    int l = 0;
    int r = node->keys.size() - 1;
    while (l <= r) {
        int m = l + (r - l) / 2;
        if (key==node->keys[m]) {
            return true;
        }
        if (key < node->keys[m]) {
            r = m - 1;
        } else {
            l = m + 1;
        }
    }
    if (node->leaf) {
        return false;
    }
    return search(node->children[l], key);
}

int BTree::find_key_position(Node *node, int key) const {
    int l = 0;
    int r = static_cast<int>(node->keys.size());
    while (l < r) {
        int m = l + (r - l) / 2;

        if (node->keys[m] < key) {
            l = m + 1;
        } else {
            r = m;
        }
    }
    return l;
}

void BTree::destroy(Node *node) {
    if (node == nullptr) {
        return;
    }
    for (Node *child: node->children) {
        destroy(child);
    }
    delete node;
}

void BTree::insert_non_full(Node *node, int key) {
    int i = find_key_position(node, key);
    if (node->leaf) {
        node->keys.insert(node->keys.begin() + i, key);
        return;
    }
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
    for (int j = middle_index + 1; j < left->keys.size(); ++j) {
        right->keys.push_back(left->keys[j]);
    }
    left->keys.resize(static_cast<std::size_t>(middle_index));
    if (!left->leaf) {
        for (int j = min_degree_; j < left->children.size(); ++j) {
            right->children.push_back(left->children[j]);
        }
        left->children.resize(min_degree_);
    }

    parent->keys.insert(parent->keys.begin() + child_index, middle_key);
    parent->children.insert(parent->children.begin() + child_index + 1, right);
}

void BTree::remove_key(Node *node, int key) {
    int key_index = 0;
    while (key_index < node->keys.size() && key > node->keys[key_index]) {
        ++key_index;
    }
    if (key_index < node->keys.size() && node->keys[key_index] == key) {
        if (node->leaf) {
            remove_from_leaf(node, key_index);
        } else {
            remove_from_non_leaf(node, key_index);
        }
        return;
    }
    if (node->leaf) {
        return;
    }

    bool key_should_be_in_last_child = key_index == node->keys.size();

    if (node->children[key_index]->keys.size() < min_degree_) {
        fill_child(node, key_index);
    }
    if (key_should_be_in_last_child && key_index > node->keys.size()) {
        remove_key(node->children[key_index - 1], key);
    } else {
        remove_key(node->children[key_index], key);
    }
}

void BTree::remove_from_leaf(Node *node, int key_index) {
    node->keys.erase(node->keys.begin() + key_index);
}

void BTree::remove_from_non_leaf(Node *node, int key_index) {
    int key = node->keys[key_index];
    if (node->children[key_index]->keys.size() >= min_degree_) {
        int predecessor = get_predecessor(node->children[key_index]);
        node->keys[key_index] = predecessor;
        remove_key(node->children[key_index], predecessor);
        return;
    }
    if (node->children[key_index + 1]->keys.size() >= min_degree_) {
        int successor = get_successor(node->children[key_index + 1]);
        node->keys[key_index] = successor;
        remove_key(node->children[key_index + 1], successor);
        return;
    }
    merge_children(node, key_index);
    remove_key(node->children[key_index], key);
}

int BTree::get_predecessor(Node *node) const {
    while (!node->leaf) {
        node = node->children.back();
    }
    return node->keys.back();
}

int BTree::get_successor(Node *node) const {
    while (!node->leaf) {
        node = node->children.front();
    }
    return node->keys.front();
}

void BTree::fill_child(Node *node, int child_index) {
    if (child_index > 0 &&
        node->children[child_index - 1]->keys.size() >= min_degree_) {
        borrow_from_previous(node, child_index);
        return;
    }
    if (child_index < node->children.size() - 1 &&
        node->children[child_index + 1]->keys.size() >= min_degree_) {
        borrow_from_next(node, child_index);
        return;
    }
    if (child_index < node->children.size() - 1) {
        merge_children(node, child_index);
    } else {
        merge_children(node, child_index - 1);
    }
}

void BTree::borrow_from_previous(Node *node, int child_index) {
    Node *child = node->children[child_index];
    Node *sibling = node->children[child_index - 1];
    child->keys.insert(child->keys.begin(), node->keys[child_index - 1]);
    if (!child->leaf) {
        child->children.insert(child->children.begin(), sibling->children.back());
        sibling->children.pop_back();
    }
    node->keys[child_index - 1] = sibling->keys.back();
    sibling->keys.pop_back();
}

void BTree::borrow_from_next(Node *node, int child_index) {
    Node *child = node->children[child_index];
    Node *sibling = node->children[child_index + 1];
    child->keys.push_back(node->keys[child_index]);
    if (!child->leaf) {
        child->children.push_back(sibling->children.front());
        sibling->children.erase(sibling->children.begin());
    }
    node->keys[child_index] = sibling->keys.front();
    sibling->keys.erase(sibling->keys.begin());
}

void BTree::merge_children(Node *node, int child_index) {
    Node *left = node->children[child_index];
    Node *right = node->children[child_index + 1];
    left->keys.push_back(node->keys[child_index]);
    for (int key: right->keys) {
        left->keys.push_back(key);
    }
    if (!left->leaf) {
        for (Node *child: right->children) {
            left->children.push_back(child);
        }
    }
    node->keys.erase(node->keys.begin() + child_index);
    node->children.erase(node->children.begin() + child_index + 1);
    delete right;
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
