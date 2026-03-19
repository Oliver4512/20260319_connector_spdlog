#!/bin/sh

# Remove the existing build directory and its contents
rm -r out/build

# Re-run cmake to generate fresh build files
# cmake -S . -B out/build
cmake -B out/build
