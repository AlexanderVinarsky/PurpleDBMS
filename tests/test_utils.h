#ifndef PURPLEDBMS_TEST_UTILS_H
#define PURPLEDBMS_TEST_UTILS_H

#include <iostream>
#include <stdexcept>
#include <string>

#define CHECK(condition) \
do { \
if (!(condition)) { \
throw std::runtime_error( \
std::string("CHECK failed: ") + #condition + \
" at " + __FILE__ + ":" + std::to_string(__LINE__) \
); \
} \
} while (false)

inline void run_test(const std::string& name, void (*test_func)()) {
    try {
        test_func();
        std::cout << "[ OK ] " << name << "\n";
    } catch (const std::exception& ex) {
        std::cerr << "[FAIL] " << name << ": " << ex.what() << "\n";
        throw;
    }
}

#endif