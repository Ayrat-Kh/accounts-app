cmake_minimum_required(VERSION VERSION 3.5 FATAL_ERROR)

project(expenso-app LANGUAGES C CXX)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(MONGO external/mongo-cxx-driver-r3.10.1/build/install)

find_package(OpenSSL REQUIRED)

# external
add_subdirectory(external)

# boost

set(BOOST_JSON_DECL ON)
set(BOOST_JSON_CLASS_DECL ON)
set(BOOST_INCLUDE_LIBRARIES beast asio json)
set(BOOST_ENABLE_CMAKE ON)
FetchContent_Declare(
  Boost
  GIT_REPOSITORY https://github.com/boostorg/boost
  GIT_TAG boost-1.84.0
)
FetchContent_MakeAvailable(Boost)



# app
file(GLOB_RECURSE SOURCES src/**/*.cpp)
add_executable(ExpensoApp
src/main.cpp
${SOURCES}
)
target_include_directories(ExpensoApp
  PRIVATE
  ${MONGO}/include/bsoncxx/v_noabi
  ${MONGO}/include/mongocxx/v_noabi
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
  ${CMAKE_CURRENT_BINARY_DIR}/${MONGO}/lib/libmongocxx.dylib
  ${CMAKE_CURRENT_BINARY_DIR}/${MONGO}/lib/libbsoncxx.dylib
  
)


# expenso-api/external/mongo-cxx-driver-r3.10.1/build/install/lib/libmongocxx.dylib

