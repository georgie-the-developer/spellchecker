#!/bin/bash
set -e

# Directories
BUILD_DIR=bin
BIN_DIR=$BUILD_DIR/
OBJ_DIR=$BUILD_DIR/obj

mkdir -p "$OBJ_DIR"

# Compile all cpp files
SRC_FILES=$(find src -name "*.cpp")

for src in $SRC_FILES; do
    obj="$OBJ_DIR/$(basename ${src%.*}).o"
    echo "Compiling $src -> $obj"
    g++ -std=c++17 -Wall -Wextra -O2 -c "$src" -o "$obj"
done

# Link
echo "Linking..."
g++ "$OBJ_DIR"/*.o -o "$BIN_DIR/main"

echo "Build complete: $BIN_DIR/main"
