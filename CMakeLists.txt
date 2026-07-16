cmake_minimum_required(VERSION 4.1)
project(PurpleDBMS)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(purpledb_core
        src/btree.cpp
        src/kv_store.cpp
        tests/test_main.cpp
        tests/test_btree.cpp
        tests/test_btree.h
        tests/test_kv_store.cpp
        include/purpledb/kv_store.h
        src/kv_store.cpp
)

target_include_directories(purpledb_core PUBLIC
        include
)

add_executable(PurpleDBMSDemo
        apps/demo.cpp
)

target_link_libraries(PurpleDBMSDemo
        purpledb_core
)

add_executable(PurpleDBMSTests
        tests/test_main.cpp
        tests/test_btree.cpp
        tests/test_kv_store.cpp
)

target_link_libraries(PurpleDBMSTests
        purpledb_core
)

enable_testing()

add_test(NAME PurpleDBMSTests COMMAND PurpleDBMSTests)