#include "purpledb/btree.h"
#include "purpledb/kv_store.h"

int main() {
    KvStore db("demo.pdb");
    db.put("x", "1");

    BTree tree(2);
    tree.insert(10);
}