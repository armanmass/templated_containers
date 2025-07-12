#include <iostream>

int main() {
    std::cout << "C++ version: " << __cplusplus << std::endl;
    if (__cplusplus == 201103L) {
        std::cout << "C++11" << std::endl;
    } else if (__cplusplus == 201402L) {
        std::cout << "C++14" << std::endl;
    } else if (__cplusplus == 201703L) {
        std::cout << "C++17" << std::endl;
    } else if (__cplusplus == 202002L) { // This is for C++20
        std::cout << "C++20" << std::endl;
    } else if (__cplusplus > 202002L) { // This will be higher for C++23, etc.
        std::cout << "C++23 or later (experimental)" << std::endl;
    } else {
        std::cout << "Older C++ standard or unknown" << std::endl;
    }
    return 0;
}