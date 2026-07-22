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
    tree.insert(5);
    tree.insert(6);
    tree.insert(12);
    tree.insert(30);
    tree.insert(7);
    tree.insert(17);

    CHECK(tree.contains(10));
    CHECK(tree.contains(20));
    CHECK(tree.contains(5));
    CHECK(tree.contains(6));
    CHECK(tree.contains(12));
    CHECK(tree.contains(30));
    CHECK(tree.contains(7));
    CHECK(tree.contains(17));

    CHECK(!tree.contains(1));
    CHECK(!tree.contains(15));
    CHECK(!tree.contains(100));
}

void test_btree_erase() {
    BTree tree(2);

    tree.insert(67);

    CHECK(tree.contains(67));

    tree.erase(67);
	CHECK(!tree.contains(67));
}

void test_btree_duplicate_insert() {
    BTree tree(2);

    tree.insert(10);
    tree.insert(10);
    tree.insert(10);

    CHECK(tree.contains(10));

    std::vector<int> keys = tree.range(0, 100);
    CHECK(keys.size() == 1);
    CHECK(keys[0] == 10);
}