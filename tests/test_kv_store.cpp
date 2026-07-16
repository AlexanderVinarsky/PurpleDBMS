#include "purpledb/kv_store.h"

#include "test_utils.h"

#include <cstdio>
#include <string>

static const char* kTestDbPath = "purpledb_test.pdb";

void test_kv_store_basic_from_old_main() {
    std::remove(kTestDbPath);
    {
        KvStore db(kTestDbPath);
        db.put("test1", "symbol:1");
        db.put("test67", "symbol:67");
        db.flush();
    }
    {
        KvStore db(kTestDbPath);
        std::string main_symbol;
        std::string print_symbol;
        CHECK(db.get("test1", main_symbol));
        CHECK(main_symbol == "symbol:1");
        CHECK(db.get("test67", print_symbol));
        CHECK(print_symbol == "symbol:67");
    }

    std::remove(kTestDbPath);
}

void test_kv_store_reopen() {
    std::remove(kTestDbPath);
    {
        KvStore db(kTestDbPath);
        db.put("test1", "symbol:1");
        db.flush();
    }
    {
        KvStore db(kTestDbPath);

        std::string value;
        CHECK(db.get("test1", value));
        CHECK(value == "symbol:1");
    }

    std::remove(kTestDbPath);
}

void test_kv_store_overwrite() {
    std::remove(kTestDbPath);
    {
        KvStore db(kTestDbPath);
        db.put("test67", "symbol:67");
        db.put("test67", "symbol:100");
        db.flush();
    }
    {
        KvStore db(kTestDbPath);
        std::string value;
        CHECK(db.get("test67", value));
        CHECK(value == "symbol:100");
    }
    std::remove(kTestDbPath);
}

void test_kv_store_missing_key() {
    std::remove(kTestDbPath);
    {
        KvStore db(kTestDbPath);
        db.put("test1", "symbol:1");
        std::string missing_symbol;
        CHECK(!db.get("missing", missing_symbol));
        db.flush();
    }
    {
        KvStore db(kTestDbPath);
        std::string missing_symbol;
        CHECK(!db.get("missing", missing_symbol));
    }
    std::remove(kTestDbPath);
}