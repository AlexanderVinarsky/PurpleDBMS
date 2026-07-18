#include "test_utils.h"

void test_btree_basic_from_old_main();
void test_btree_split();
void test_btree_erase();

void test_kv_store_basic_from_old_main();
void test_kv_store_reopen();
void test_kv_store_overwrite();
void test_kv_store_missing_key();

int main() {
    run_test("btree basic from old main", test_btree_basic_from_old_main);
    run_test("btree split", test_btree_split);
    run_test("btree erase", test_btree_erase);

    run_test("kv_store basic from old main", test_kv_store_basic_from_old_main);
    run_test("kv_store reopen", test_kv_store_reopen);
    run_test("kv_store overwrite", test_kv_store_overwrite);
    run_test("kv_store missing key", test_kv_store_missing_key);

    return 0;
}