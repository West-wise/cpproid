#!/bin/bash

ARCHITECTURE=$1

# Clean up previous build-related files
rm -rf CMakeCache.txt CMakeFiles cmake_install.cmake Makefile

if [ -z $ARCHITECTURE ]; then
    cmake .
    exit 1
fi

if [ $ARCHITECTURE == "arm64" ]; then
    if ! cmake -DCMAKE_TOOLCHAIN_FILE=$AARCH64 .; then
        echo "CMake Error: Failed to configure the arm64 build"
        exit 1
    fi
    exit 1
fi

if [ $ARCHITECTURE == "arm32" ]; then
    if ! cmake -DCMAKE_TOOLCHAIN_FILE=$ARMV7A .; then
        echo "CMake Error: Failed to configure the arm32 build"
        exit 1
    fi
    exit 1
fi

# If an unsupported architecture is provided
echo "Error: Unsupported architecture specified"
exit 1
