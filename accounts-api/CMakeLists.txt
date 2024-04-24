cmake_minimum_required(VERSION VERSION 3.5 FATAL_ERROR)

project(accounts-api LANGUAGES C CXX)

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
  GIT_SUBMODULES "
  libs/coroutine
  libs/date_time
  libs/endian
  libs/exception
  libs/integer
  libs/logic
  libs/pool
  libs/tti
  libs/smart_ptr
  libs/optional
  libs/type_traits
  libs/algorithm
  libs/tuple
  libs/numeric/conversion
  libs/function
  libs/functional
  libs/typeof
  libs/random
  libs/unordered
  libs/mpl
  libs/callable_traits
  libs/tokenizer
  libs/function_types
  libs/fusion
  libs/regex
  libs/array
  libs/static_string
  libs/utility
  libs/io
  libs/detail
  libs/concept_check
  libs/preprocessor
  libs/lexical_cast
  libs/iterator
  libs/range
  libs/conversion
  libs/context
  libs/bind
  libs/core
  libs/container
  libs/align
  libs/assert
  libs/config
  libs/mp11
  libs/json
  libs/describe
  libs/asio
  libs/beast
  libs/system
  libs/uuid
  libs/process
  libs/filesystem
  libs/intrusive
  libs/throw_exception
  libs/move
  libs/container_hash
  libs/static_assert
  libs/variant2
  libs/winapi
  libs/predef
  tools/cmake"
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

ExternalProject_Add(
  mongo_content
  GIT_REPOSITORY https://github.com/mongodb/mongo-cxx-driver
  GIT_TAG r3.10.1
  BUILD_COMMAND ${CMAKE_COMMAND} . -DCMAKE_BUILD_TYPE=Release -DENABLE_BSONCXX_POLY_USE_IMPLS=1 -DENABLE_TESTS=OFF
  COMMAND ${CMAKE_COMMAND} --build .
)
ExternalProject_Get_property(mongo_content BINARY_DIR)
set(MONGO_INSTALL_DIR ${BINARY_DIR})

file(GLOB_RECURSE SOURCES src/**/*.cpp)
  add_executable(AccountsApi
  src/accounts/main.cpp
  ${SOURCES}
)
target_include_directories(AccountsApi
  PRIVATE
  ${MONGO_INSTALL_DIR}/install/include/bsoncxx/v_noabi
  ${MONGO_INSTALL_DIR}/install/include/mongocxx/v_noabi
  ${boost_SOURCE_DIR}/libs/uuid/include
  ${boost_SOURCE_DIR}/libs/callable_traits/include
  ${boost_SOURCE_DIR}/libs/process/include
  ${boost_SOURCE_DIR}/libs/filesystem/include
  ${boost_SOURCE_DIR}/libs/random/include
  ${boost_SOURCE_DIR}/libs/tti/include
  ${cppjwt_content_SOURCE_DIR}/include
  ${CMAKE_CURRENT_BINARY_DIR}/src
  src/
)
target_link_libraries(AccountsApi
  PRIVATE 
  Boost::beast
  Boost::asio
  Boost::json
  uWebSockets
  OpenSSL::SSL
  ${MONGO_INSTALL_DIR}/install/lib/libmongocxx${LIB_EXT}
  ${MONGO_INSTALL_DIR}/install/lib/libbsoncxx${LIB_EXT}
)
