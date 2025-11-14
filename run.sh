#!/bin/bash

# Run script for Soldat
set -e  # Exit immediately if a command exits with a non-zero status

echo "Running Soldat..."

# Check if build directory exists and has the executable
if [ ! -d "build" ]; then
    echo "Error: build directory does not exist. Please run build.sh first."
    exit 1
fi

# Change to build directory
cd build

# Look for the Soldat client executable and run it
if [ -f "./soldat_client" ]; then
    echo "Starting Soldat client..."
    ./soldat_client "$@"
elif [ -f "./soldat" ]; then
    echo "Starting Soldat..."
    ./soldat "$@"
elif [ -f "./Soldat" ]; then
    echo "Starting Soldat..."
    ./Soldat "$@"
else
    echo "Error: Soldat executable not found in build directory."
    echo "Available files in build directory:"
    ls -la
    exit 1
fi