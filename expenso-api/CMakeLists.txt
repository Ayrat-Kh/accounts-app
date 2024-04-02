cmake_minimum_required(VERSION VERSION 3.5 FATAL_ERROR)

project(expenso-app LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# set(CMAKE_C_COMPILER /opt/homebrew/opt/llvm/bin/clang)
# set(CMAKE_CXX_COMPILER /opt/homebrew/opt/llvm/bin/clang++)

add_executable(ExpensoApp
  src/main.cpp
)
include(FetchContent)

# qxorm
# file(GLOB_RECURSE SOURCES ${mongo_content_SOURCE_DIR}/src/*.cpp)
# add_library(mongo ${SOURCES})
# target_include_directories(mongo PUBLIC
#   ${mongo_content_SOURCE_DIR}/src/bsoncxx/include/bsoncxx/v_noabi 
#   ${mongo_content_BINARY_DIR}/src/bsoncxx/lib/bsoncxx/v_noabi
#   ${mongo_content_SOURCE_DIR}/src/mongocxx/include/mongocxx/v_noabi
#   ${mongo_content_BINARY_DIR}/src/mongocxx/lib/mongocxx/v_noabi
# )

# # export(TARGETS tabulate)

# set(CMAKE_BUILD_TYPE:STRING Debug)
# set(CMAKE_INSTALL_PREFIX:PATH ${TinyORM_BINARY_DIR})
# # set(BUILD_SHARED_LIBS OFF)
# set(VERBOSE_CONFIGURE:BOOL ON)
# set(BUILD_TESTS:BOOL OFF)
# set(MYSQL_PING:BOOL OFF)
# set(TOM OFF)
# set(USE_CPP17 ON)
# set(TOM_EXAMPLE:BOOL OFF)
# set(MATCH_EQUAL_EXPORTED_BUILDTREE:BOOL ON)
# FetchContent_GetProperties(tabulate)
# FetchContent_MakeAvailable(TinyORM)

# # external
add_subdirectory(external)

set(MONGO external/mongo-cxx-driver-r3.10.1/build/install)

target_include_directories(ExpensoApp
  PRIVATE
  ${MONGO}/include/bsoncxx/v_noabi
  ${MONGO}/include/mongocxx/v_noabi
)
target_link_libraries(ExpensoApp
  PRIVATE 
  # Boost::asio
  # Boost::beast
  # Boost::filesystem
  # Boost::thread
  # Boost::program_options
  uWebSockets
  # range-v3
  # tabulate::tabulate
  # TinyOrm::CommonConfig
  # "/Users/airatkhisamiev/dev/expenso-app/expenso-api/external/mongo-cxx-driver-r3.10.1/build/install/lib/libmongocxx.dylib"
  ${CMAKE_CURRENT_BINARY_DIR}/${MONGO}/lib/libmongocxx.dylib
  ${CMAKE_CURRENT_BINARY_DIR}/${MONGO}/lib/libbsoncxx.dylib
)


# expenso-api/external/mongo-cxx-driver-r3.10.1/build/install/lib/libmongocxx.dylib

