#!/bin/bash

# Run asteroids with GDB and get stack trace on crash
# Usage: ./run_asteroids_gdb.sh

# Build the asteroids demo first
make demos/asteroids

# Run with GDB in batch mode to get stack trace on crash
echo "Running asteroids with GDB (will exit on crash with stack trace)..."
gdb --batch --ex run --ex bt --args ./demos/asteroids
