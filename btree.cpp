#include <iostream>
#include <vector>
#include <algorithm>

class BTree {
private:
    struct Node {
        bool leaf;
        std::vector<int> keys;
        std::vector<Node*> children;

        explicit Node(bool is_leaf) : leaf(is_leaf) {}
    };

    Node* root_;
    int min_degree_;

public:
    explicit BTree(int min_degree)
        : root_(new Node(true)), min_degree_(min_degree) {
    }

    ~BTree() {
        destroy(root_);
    }

    bool contains(int key) const {
        return true;//search(root_, key);
    }

private:
    int max_keys() const {
        return 2 * min_degree_ - 1;
    }

    bool is_full(Node* node) const {
        return static_cast<int>(node->keys.size()) == max_keys();
    }

    void destroy(Node* node) {
        if (node == nullptr) {
            return;
        }

        for (Node* child : node->children) {
            destroy(child);
        }

        delete node;
    }

    void insert_non_full(Node* node, int key) {
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

    void split_child(Node* parent, int child_index) {
        Node* left = parent->children[child_index];
        Node* right = new Node(left->leaf);

        int middle_index = min_degree_ - 1;
        int middle_key = left->keys[middle_index];
        for (int j = middle_index + 1; j < left->keys.size(); ++j) {
            right->keys.push_back(left->keys[j]);
        }
        left->keys.resize(middle_index);
        if (!left->leaf) {
            for (int j = min_degree_; j < left->children.size(); ++j) {
                right->children.push_back(left->children[j]);
            }

            left->children.resize(min_degree_);
        }

        parent->keys.insert(parent->keys.begin() + child_index, middle_key);
        parent->children.insert(parent->children.begin() + child_index + 1, right);
    }
};
