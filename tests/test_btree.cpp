#include "purpledb/btree.h"

#include "test_utils.h"

void test_btree_basic_from_old_main() {
    BTree tree(2);

    tree.insert(10);
    CHECK(tree.contains(10));
    CHECK(!tree.contains(67));

    tree.insert(67);
    tree.insert(5);

    CHECK(tree.contains(10));
    CHECK(tree.contains(67));
    CHECK(tree.contains(5));
    CHECK(!tree.contains(100));
}

void test_btree_split() {
    BTree tree(2);

    tree.insert(10);
    tree.insert(20);


    CHECK(tree.contains(10));
    CHECK(tree.contains(20));

    CHECK(!tree.contains(1));
}

void test_btree_erase() {
    BTree tree(2);

    tree.insert(67);

    CHECK(tree.contains(67));

    CHECK(tree.erase(67));
    CHECK(!tree.contains(67));
}