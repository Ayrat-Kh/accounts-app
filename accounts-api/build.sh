#!/bin/sh

SCRIPT_DIR=$(dirname -- "$( readlink -f -- "$0"; )";)

cd build
cmake ..
cmake --build .
