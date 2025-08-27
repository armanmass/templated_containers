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
./build_all.sh
```

### Step 4: Run the Tests

```bash
./build/Release/all_tests
```

Expected output:

```
[==========] Running 25 tests from 3 test suites.
[----------] Global test environment set-up.
[----------] 12 tests from VectorTest
[ RUN      ] VectorTest.DefaultConstructor
[       OK ] VectorTest.DefaultConstructor (0 ms)
[ RUN      ] VectorTest.Data
[       OK ] VectorTest.Data (0 ms)
[ RUN      ] VectorTest.PushBack
[       OK ] VectorTest.PushBack (0 ms)
...
[ RUN      ] PopulatedVectorTest.Clear
[       OK ] PopulatedVectorTest.Clear (0 ms)
[ RUN      ] PopulatedVectorTest.ShrinkToFit
[       OK ] PopulatedVectorTest.ShrinkToFit (0 ms)
[----------] 8 tests from PopulatedVectorTest (0 ms total)

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

[----------] Global test environment tear-down
[==========] 25 tests from 3 test suites ran. (0 ms total)
[  PASSED  ] 25 tests.

```

Note: only forward iteration is supported for hive currently