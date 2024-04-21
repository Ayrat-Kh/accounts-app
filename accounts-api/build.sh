#!/bin/sh

# SCRIPT_DIR=$(dirname -- "$( readlink -f -- "$0"; )";)

# cd build
# cmake ..


bash -c "cd build; cmake --build -DCMAKE_C_COMPILER="/opt/homebrew/opt/llvm/bin/clang" -DCMAKE_CXX_COMPILER="/opt/homebrew/opt/llvm/bin/clang++" .;"