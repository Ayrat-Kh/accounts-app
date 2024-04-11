#!/bin/sh

SCRIPT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd)

# prepare build dir
mkdir build
cd build
cmake .. -DSKIP_BUILD=ON

# build mongo c
MONGOC_SOURCE=${SCRIPT_DIR}/build/_deps/mongoc_content-src
MONGOC_INSTALL=${MONGOC_SOURCE}/build/install
cd ${MONGOC_SOURCE}/build
cmake ..
cmake --build .
cmake --install . --prefix ${MONGOC_INSTALL}

# build mongo cxx
MONGOCXX_SOURCE=${SCRIPT_DIR}/build/_deps/mongo_content-src
MONGOCXX_INSTALL=${MONGOCXX_SOURCE}/build/install
cd ${MONGOCXX_SOURCE}/build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBSONCXX_POLY_USE_IMPLS=ON -DCMAKE_PREFIX_PATH:PATH=${MONGOC_INSTALL}
cmake --build .
cmake --install . --prefix ${MONGOCXX_INSTALL}
