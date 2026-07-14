cmake_minimum_required(VERSION 4.1)
project(PurpleDBMS)

set(CMAKE_CXX_STANDARD 14)

add_executable(PurpleDBMS
        main.cpp
        btree.cpp
        btree.h)
