#!/bin/sh

SCRIPT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd)

cd build
cmake ..
cmake --build .

ls -la
