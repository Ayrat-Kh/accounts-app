cmake_minimum_required(VERSION VERSION 3.26...3.28 FATAL_ERROR)

project(expenso-app LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(ExpensoApp
  src/main.cpp
)
include(FetchContent)

#boost
# set(BOOST_INCLUDE_LIBRARIES thread filesystem beast system program_options asio date_time)
# set(BOOST_ENABLE_CMAKE ON)
# set(BOOST_USE_STATIC_LIBS ON)
# set(BOOST_USE_MULTITHREADED ON)
# set(BOOST_USE_STATIC_RUNTIME OFF)

# FetchContent_Declare(
#   Boost
#   GIT_REPOSITORY https://github.com/boostorg/boost.git
#   GIT_PROGRESS TRUE
#   GIT_TAG boost-1.84.0
# )
# FetchContent_MakeAvailable(Boost)

# uwebsockets
add_subdirectory(external)


target_link_libraries(ExpensoApp
  #PRIVATE 
  # Boost::asio
  # Boost::beast
  # Boost::filesystem
  # Boost::thread
  # Boost::program_options
  uWebSockets
)




