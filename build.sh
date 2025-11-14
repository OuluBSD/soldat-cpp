#!/bin/bash

# Build script for Soldat
set -e  # Exit immediately if a command exits with a non-zero status

echo "Building Soldat..."

# Create required directories if they don't exist
if [ ! -d "assets" ]; then
    echo "Creating assets directory..."
    mkdir -p assets
fi

if [ ! -d "configs" ]; then
    echo "Creating configs directory..."
    mkdir -p configs
fi

if [ ! -d "maps" ]; then
    echo "Creating maps directory..."
    mkdir -p maps
fi

# Clean and create build directory
echo "Cleaning build directory..."
rm -rf build
mkdir -p build

# Change to build directory
cd build

# Run cmake to configure the project
echo "Running cmake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
echo "Running make..."
make -j$(nproc)

echo "Build completed successfully!"