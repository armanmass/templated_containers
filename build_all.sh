#!/bin/bash

set -e

# Release build
echo "Building Release..."
conan install . --build=missing
cmake --preset conan-release
cmake --build --preset conan-release

# Debug builds with different sanitizers
sanitizers=("asan" "tsan" "msan" "ubsan")
# avoid making debug temp
sanitizers=()

for sanitizer in "${sanitizers[@]}"; do
   echo "Building Debug with $sanitizer..."
   conan install . --settings=build_type=Debug --options=sanitizer="$sanitizer --build=missing"
   cmake --preset conan-debug
   cmake --build --preset conan-debug
   
   # Rename executables with sanitizer suffix
    cd build/Debug
    if [[ -f "tests" ]]; then
        sanitizer_clean="${sanitizer//+/_}"
        mv "tests" "tests_${sanitizer_clean}"
        echo "Renamed: tests -> tests_${sanitizer_clean}"
    fi
    cd ../..
done

echo ""
echo "All builds done!"
echo "Release executables: build/Release/"
echo "Debug executables with sanitizer suffixes: build/Debug/"
ls -la build/Debug/