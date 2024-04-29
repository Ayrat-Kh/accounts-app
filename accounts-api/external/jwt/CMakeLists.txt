cmake_minimum_required(VERSION VERSION 3.5 FATAL_ERROR)

# jwt
FetchContent_Declare(
  cppjwt_content
  GIT_REPOSITORY https://github.com/Thalhammer/jwt-cpp
  GIT_TAG v0.7.0
)
FetchContent_MakeAvailable(cppjwt_content)

add_library(jwt INTERFACE)
target_include_directories(jwt
  INTERFACE
  ${cppjwt_content_SOURCE_DIR}/include
)
