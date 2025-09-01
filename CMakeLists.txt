cmake_minimum_required(VERSION 4.1)
project(TemplatedContainers)

set(CMAKE_CXX_SCAN_FOR_MODULES 1)
find_package(GTest REQUIRED)

add_executable(
    all_tests
    tests/testvec.cpp
    tests/testhive.cpp
)

target_include_directories(
    all_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    all_tests
    PRIVATE
        GTest::gtest_main
)