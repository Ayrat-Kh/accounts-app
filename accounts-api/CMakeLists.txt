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

file(GLOB_RECURSE SOURCES src/**/*.cpp)
  add_executable(AccountsApi
  src/accounts/main.cpp
  ${SOURCES}
)
target_include_directories(AccountsApi
  PRIVATE
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
  mongo
  jwt
)
