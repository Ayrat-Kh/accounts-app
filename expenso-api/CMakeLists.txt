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
# set(BOOST_JSON_DECL ON)
# set(BOOST_JSON_CLASS_DECL ON)
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

#mongo
FetchContent_Declare(
  mongo_content
  GIT_REPOSITORY https://github.com/mongodb/mongo-cxx-driver
  GIT_TAG r3.10.1
)
FetchContent_MakeAvailable(mongo_content)
ExternalProject_Add(
  mongo_content
  SOURCE_DIR ${mongo_content_SOURCE_DIR}
  BUILD_COMMAND ${CMAKE_COMMAND} . -DCMAKE_BUILD_TYPE=Release -DENABLE_BSONCXX_POLY_USE_IMPLS=1
  COMMAND ${CMAKE_COMMAND} --build .
  COMMAND ${CMAKE_COMMAND} --build . --target install
)
ExternalProject_Get_property(mongo_content BINARY_DIR)
set(MONGO_INSTALL_DIR ${BINARY_DIR})

# app
file(GLOB_RECURSE SOURCES src/**/*.cpp)
add_executable(ExpensoApp
  src/accounts/main.cpp
  ${SOURCES}
)
target_include_directories(ExpensoApp
  PRIVATE
  ${MONGO_INSTALL_DIR}/install/include/bsoncxx/v_noabi
  ${MONGO_INSTALL_DIR}/install/include/mongocxx/v_noabi
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
  ${MONGO_INSTALL_DIR}/install/lib/libmongocxx${LIB_EXT}
  ${MONGO_INSTALL_DIR}/install/lib/libbsoncxx${LIB_EXT}
)