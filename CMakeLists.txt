cmake_minimum_required(VERSION 3.14)
project(TemplatedContainers)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)

enable_testing()

# Vector Test

add_executable(
    all_tests
    tests/testvec.cpp
    tests/testht.cpp
    tests/testhive.cpp
)

target_include_directories(
    all_tests
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    all_tests
    PRIVATE
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(all_tests)