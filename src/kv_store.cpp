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

void test_kv_store_size_empty() {
    std::remove(kTestDbPath);
    {
        KvStore db(kTestDbPath);

        CHECK(db.empty());
        CHECK(db.size() == 0);

        db.put("test1", "symbol:1");
        CHECK(!db.empty());
        CHECK(db.size() == 1);

        db.put("test67", "symbol:67");
        CHECK(!db.empty());
        CHECK(db.size() == 2);

        db.flush();
    }
    std::remove(kTestDbPath);
}

void test_kv_store_erase() {
    std::remove(kTestDbPath);
    {
        KvStore db(kTestDbPath);

        db.put("test1", "symbol:1");

        std::string value;
        CHECK(db.get("test1", value));
        CHECK(value == "symbol:1");

        db.erase("test1");
        CHECK(!db.get("test1", value));
        CHECK(db.empty());

        db.flush();
    }
    std::remove(kTestDbPath);
}

void test_kv_store_erase_reopen() {
    std::remove(kTestDbPath);
    {
        KvStore db(kTestDbPath);

        db.put("test1", "symbol:1");
        db.put("test67", "symbol:67");
        db.erase("test1");
        db.flush();
    }
    {
        KvStore db(kTestDbPath);

        std::string value;
        CHECK(!db.get("test1", value));

        CHECK(db.get("test67", value));
        CHECK(value == "symbol:67");

        CHECK(db.size() == 1);
    }
    std::remove(kTestDbPath);
}

void test_kv_store_compact() {
    std::remove(kTestDbPath);
    {
        KvStore db(kTestDbPath);

        db.put("test1", "symbol:1");
        db.put("test67", "symbol:67");
        db.erase("test67");

        CHECK(db.size() == 1);

        db.compact();

        std::string value;
        CHECK(db.get("test1", value));
        CHECK(!db.get("test67", value));
        CHECK(db.size() == 1);

        db.flush();
    }
    {
        KvStore db(kTestDbPath);

        std::string value;
        CHECK(db.get("test1", value));
        CHECK(value == "symbol:100");
        CHECK(!db.get("test67", value));
        CHECK(db.size() == 1);
    }
    std::remove(kTestDbPath);
}
