#include "purpledb/kv_store.h"
#include "test_utils.h"
#include <cstdio>
#include <string>

static const char* kTestDbPath = "purpledb_test.pdb";
void test_kv_store_put_get() {
    std::remove(kTestDbPath);

    KvStore db(kTestDbPath);

    db.put("name:main", "symbol:1");
    std::string value;
    CHECK(db.get("name:main", value));
    CHECK(value == "symbol:1");
    CHECK(!db.get("name:missing", value));
    std::remove(kTestDbPath);
}

void test_kv_store_reopen() {
    std::remove(kTestDbPath);
    {
        KvStore db(kTestDbPath);
        db.put("name:main", "symbol:1");
        db.flush();
    }
    {
        KvStore db(kTestDbPath);
        std::string value;
        CHECK(db.get("name:main", value));
        CHECK(value == "symbol:1");
    }
    std::remove(kTestDbPath);
}

void test_kv_store_overwrite() {
    std::remove(kTestDbPath);
    {
        KvStore db(kTestDbPath);
        db.put("name:print", "symbol:42");
        db.put("name:print", "symbol:100");
        std::string value;
        CHECK(db.get("name:print", value));
        CHECK(value == "symbol:100");
        db.flush();
    }
    {
        KvStore db(kTestDbPath);
        std::string value;
        CHECK(db.get("name:print", value));
        CHECK(value == "symbol:100");
    }
    std::remove(kTestDbPath);
}