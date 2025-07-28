# Advanced C++ Data Structures

A C++20 header-only library of custom data structures, featuring a high-performance `hive` container. This collection is designed for educational purposes and performance-critical applications. (also included are implementations of `Vector`, `HashTable`, and `BST`.)

The primary focus is the `hive` container which maintains iterator validation upon insertion and deletion. `std::hive` which will be introduced in the c++ 26 standard is the main motivator.

## Building and Running

### Step 1: Clone Repo

```bash
git clone https://github.com/armanmass/templated_containers.git
```

### Step 2: Prerequisites

Ensure you have a C++20 compatible compiler (like GCC 13+ or Clang 18+), CMake (3.10+), and Git installed. The project uses GoogleTest for unit testing, which will be fetched automatically by CMake.

### Step 3: Build the Project

```bash
mkdir build
cd build
cmake ..
make
```

### Step 4: Run the Tests

```bash
# From the 'build' directory
./all_tests
```

Expected output:

```
[==========] Running 34 tests from 6 test suites.
[----------] Global test environment set-up.
[----------] 5 tests from HiveTest
[ RUN      ] HiveTest.DefaultConstruct
[       OK ] HiveTest.DefaultConstruct (0 ms)
[ RUN      ] HiveTest.EmplaceAndGrow
[       OK ] HiveTest.EmplaceAndGrow (0 ms)
[ RUN      ] HiveTest.Iterator
[       OK ] HiveTest.Iterator (0 ms)
[ RUN      ] HiveTest.FreeListTest
[       OK ] HiveTest.FreeListTest (0 ms)
[ RUN      ] HiveTest.Clear
[       OK ] HiveTest.Clear (0 ms)
[----------] 5 tests from HiveTest (0 ms total)

[----------] 9 tests from HashTableTest
... etc ...
[----------] 20 tests from VectorTest
... etc ...
[----------] Global test environment tear-down
[==========] 34 tests from 6 test suites ran. (1 ms total)
[  PASSED  ] 34 tests.
```

Note: only forward iteration is currently supported.