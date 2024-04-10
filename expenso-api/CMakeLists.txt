cmake_minimum_required(VERSION VERSION 3.5 FATAL_ERROR)

project(expenso-app LANGUAGES C CXX)

include(FetchContent)

set(LIBCXX_ENABLE_INCOMPLETE_FEATURES ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(MONGO external/mongo-cxx-driver-r3.10.1/build/install)

if(WIN32)
  set(LIB_EXT ".dll")
elseif(APPLE)
  set(LIB_EXT ".dylib")
elseif(UNIX)
  set(LIB_EXT ".so")
endif()

find_package(OpenSSL REQUIRED)

# external
add_subdirectory(external)

# boost
set(BOOST_INCLUDE_LIBRARIES beast asio json)
set(BOOST_ENABLE_CMAKE ON)
FetchContent_Declare(
  Boost
  GIT_REPOSITORY https://github.com/boostorg/boost
  GIT_TAG boost-1.84.0
)
FetchContent_MakeAvailable(Boost)

# jwt
FetchContent_Declare(
  cppjwt_content
  GIT_REPOSITORY https://github.com/Thalhammer/jwt-cpp
  GIT_TAG v0.7.0
)
FetchContent_MakeAvailable(cppjwt_content)

# mongo prefetch
FetchContent_Declare(
  mongoc_content
  GIT_REPOSITORY https://github.com/mongodb/mongo-c-driver
  GIT_TAG 1.25.0
)
FetchContent_Populate(mongoc_content)

FetchContent_Declare(
  mongo_content
  GIT_REPOSITORY https://github.com/mongodb/mongo-cxx-driver
  GIT_TAG r3.10.1
)
FetchContent_Populate(mongo_content)

# app
file(GLOB_RECURSE SOURCES src/**/*.cpp)
add_executable(ExpensoApp
  src/accounts/main.cpp
  ${SOURCES}
)
target_include_directories(ExpensoApp
  PRIVATE
  ${mongo_content_SOURCE_DIR}/build/install/include/bsoncxx/v_noabi
  ${mongo_content_SOURCE_DIR}/build/install/include/mongocxx/v_noabi
  ${cppjwt_content_SOURCE_DIR}/include
  ${CMAKE_CURRENT_BINARY_DIR}/src
  src/
)
target_link_libraries(ExpensoApp
  PRIVATE 
  Boost::beast
  Boost::asio
  Boost::json
  # Boost::program_options
  uWebSockets
  OpenSSL::SSL
  ${mongo_content_SOURCE_DIR}/build/install/lib/libmongocxx${LIB_EXT}
  ${mongo_content_SOURCE_DIR}/build/install/lib/libbsoncxx${LIB_EXT}
)