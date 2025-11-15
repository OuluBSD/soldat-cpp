#!/bin/bash

# Build script for Soldat
set -e  # Exit immediately if a command exits with a non-zero status

echo "Building Soldat..."

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir build
fi

# Change to build directory
cd build

# Check if we need to run cmake for the first time
if [ ! -f "Makefile" ] && [ ! -f "build.ninja" ]; then
    echo "Configuring with CMake..."
    cmake .. -DCMAKE_BUILD_TYPE=Debug
fi

echo "Compiling..."
make -j$(nproc)

echo "Build completed successfully!"
echo "You can now run the game with ./run.sh"