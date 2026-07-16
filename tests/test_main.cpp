#include "test_utils.h"

void test_btree_empty_tree();
void test_btree_insert_and_contains();
void test_btree_split_root();
void test_btree_erase();

void test_kv_store_put_get();
void test_kv_store_reopen();
void test_kv_store_overwrite();

int main() {
    run_test("btree empty tree", test_btree_empty_tree);
    run_test("btree insert and contains", test_btree_insert_and_contains);
    run_test("btree split root", test_btree_split_root);
    run_test("btree erase", test_btree_erase);

    run_test("kv_store put/get", test_kv_store_put_get);
    run_test("kv_store reopen", test_kv_store_reopen);
    run_test("kv_store overwrite", test_kv_store_overwrite);

    return 0;
}