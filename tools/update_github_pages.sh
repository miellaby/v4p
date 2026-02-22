#!/bin/bash

# Script to update GitHub Pages site versioned in the docs folder
# This builds the HTML demos and copies them to the docs folder

set -e  # Exit on error

echo "=== Updating GitHub Pages ==="

# Change to the project root directory
cd "$(dirname "$0")/.."

echo "Cleaning previous build..."
make clean

echo "Building demos for Emscripten with bitmap backend..."
# Build specific demos with TARGET=emscripten and BACKEND=bitmap
# Build them one by one to avoid timeout issues
for demo in asteroids starfield_3d car_track; do
    echo "Building ${demo}..."
    make TARGET=emscripten BACKEND=bitmap "demos/${demo}.html" V=1
    echo "Built ${demo}"
done

echo "Creating docs directory if it doesn't exist..."
mkdir -p docs

echo "Copying generated files to docs folder..."
# Copy HTML, JS, and WASM files for each demo
for demo in asteroids starfield_3d car_track; do
    if [ -f "demos/${demo}.html" ]; then
        echo "Copying ${demo} files..."
        cp "demos/${demo}.html" "docs/"
        cp "demos/${demo}.js" "docs/"
        cp "demos/${demo}.wasm" "docs/"
    else
        echo "Warning: demos/${demo}.html not found"
    fi
done

echo "Cleaning up build artifacts..."
make clean

echo "=== GitHub Pages update complete ==="
echo "The following files were updated in docs/:"
ls -la docs/*.html docs/*.js docs/*.wasm 2>/dev/null || echo "No files found"
