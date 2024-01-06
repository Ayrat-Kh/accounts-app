cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

project(app)

set(SOURCE_FILES src/app.cpp)
add_executable(app ${SOURCE_FILES})

# if(NOT CMAKE_BUILD_TYPE)
#       set(CMAKE_BUILD_TYPE "Release")
# endif()

# message(STATUS "Building in ${CMAKE_BUILD_TYPE} mode...")




# set(POCO_STATIC ON)
add_subdirectory(lib/poco)
target_link_libraries(app PUBLIC JWT)
