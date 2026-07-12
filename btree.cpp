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
};
