#!/bin/bash

# Run asteroids with GDB and get stack trace on crash
# Usage: ./run_asteroids_gdb.sh [asan]

# Check if ASAN mode is requested
ASAN_MODE=""
if [ "$1" = "asan" ]; then
    ASAN_MODE="DEBUG=1 ASAN=1"
    echo "Building with AddressSanitizer..."
fi

# Build the asteroids demo first
make clean
make $ASAN_MODE demos/asteroids

# Set ASAN environment variables if in ASAN mode
if [ "$ASAN_MODE" != "" ]; then
    export ASAN_OPTIONS=detect_leaks=1:abort_on_error=1:symbolize=1:verbosity=1
    export ASAN_SYMBOLIZER_PATH=$(which llvm-symbolizer 2>/dev/null || echo "")
    echo "ASAN options set: detect_leaks=1:abort_on_error=1:symbolize=1:verbosity=1"
fi

# Run with GDB in batch mode to get stack trace on crash
echo "Running asteroids with GDB (will exit on crash with stack trace)..."
gdb --batch --ex run --ex bt --args ./demos/asteroids
