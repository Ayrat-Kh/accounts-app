cmake_minimum_required(VERSION VERSION 3.5)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
  uSockets_content
  GIT_REPOSITORY https://github.com/uNetworking/uSockets
  GIT_TAG v0.8.8
  GIT_SHALLOW ON
  GIT_SUBMODULES ""
)
FetchContent_MakeAvailable(uSockets_content)
file(GLOB_RECURSE SOURCES ${usockets_content_SOURCE_DIR}/src/*.c)

add_library(uSockets ${SOURCES} ${usockets_content_SOURCE_DIR}/src/eventing/asio.cpp)
target_link_libraries(uSockets
  PRIVATE 
  Boost::asio
)
target_include_directories(uSockets PUBLIC ${usockets_content_SOURCE_DIR}/src)
# target_compile_definitions(uSockets PRIVATE )
target_compile_definitions(uSockets PRIVATE LIBUS_NO_SSL LIBUS_USE_ASIO)