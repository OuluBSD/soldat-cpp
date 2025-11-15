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

# Run cmake and make
echo "Running cmake..."
cmake ..

echo "Running make..."
make

echo "Build completed successfully!"