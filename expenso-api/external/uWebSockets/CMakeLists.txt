cmake_minimum_required(VERSION VERSION 3.26...3.28 FATAL_ERROR)
include(FetchContent)
FetchContent_Declare(
  uWebSockets_content
  GIT_REPOSITORY https://github.com/uNetworking/uWebSockets
  GIT_TAG v20.62.0
  GIT_SHALLOW ON
  GIT_SUBMODULES ""
)
SET(LIBUS_USE_LIBUV 1)
FetchContent_MakeAvailable(uWebSockets_content)
find_package(ZLIB REQUIRED)
add_library(uWebSockets INTERFACE)
target_include_directories(uWebSockets INTERFACE ${uwebsockets_content_SOURCE_DIR}/src/)
target_link_libraries(uWebSockets INTERFACE uSockets ${ZLIB_LIBRARIES})