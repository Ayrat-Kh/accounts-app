cmake_minimum_required(VERSION VERSION 3.5 FATAL_ERROR)

project(expenso-app LANGUAGES C CXX)

include(FetchContent)

set(LIBCXX_ENABLE_INCOMPLETE_FEATURES ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(MONGO external/mongo-cxx-driver-r3.10.1/build/install)

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
set(CMAKE_INSTALL_LOCAL_ONLY ON)
FetchContent_Declare(
  mongo_content
  GIT_REPOSITORY https://github.com/mongodb/mongo-cxx-driver
  GIT_TAG r3.10.1
)
FetchContent_MakeAvailable(mongo_content)

# app
file(GLOB_RECURSE SOURCES src/**/*.cpp)
add_executable(ExpensoApp
src/main.cpp
${SOURCES}
)
# expenso-api/_deps/mongo_content-build/
target_include_directories(ExpensoApp
  PRIVATE
  ${mongo_content_SOURCE_DIR}/src/bsoncxx/include/bsoncxx/v_noabi
  ${mongo_content_SOURCE_DIR}/src/mongocxx/include/mongocxx/v_noabi
  ${mongo_content_BINARY_DIR}/src/bsoncxx/lib/bsoncxx/v_noabi
  ${mongo_content_BINARY_DIR}/src/mongocxx/lib/mongocxx/v_noabi
  ${cppjwt_content_SOURCE_DIR}/include
  ${CMAKE_CURRENT_BINARY_DIR}/src
)
target_link_libraries(ExpensoApp
  PRIVATE 
  Boost::beast
  Boost::asio
  Boost::json
  # Boost::filesystem
  # Boost::thread
  # Boost::program_options
  uWebSockets
  OpenSSL::SSL
  # range-v3
  # tabulate::tabulate
  # TinyOrm::CommonConfig
  # "/Users/airatkhisamiev/dev/expenso-app/expenso-api/external/mongo-cxx-driver-r3.10.1/build/install/lib/libmongocxx.dylib"
  ${mongo_content_SOURCE_DIR}
  ${mongo_content_BINARY_DIR}/src/bsoncxx/libbsoncxx.dylib
  ${mongo_content_BINARY_DIR}/src/mongocxx/libmongocxx.dylib
  # ${CMAKE_CURRENT_BINARY_DIR}/${MONGO}/lib/
  # ${CMAKE_CURRENT_BINARY_DIR}/${MONGO}/lib/
  
)


# expenso-api/external/mongo-cxx-driver-r3.10.1/build/install/lib/libmongocxx.dylib

