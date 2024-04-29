cmake_minimum_required(VERSION VERSION 3.5 FATAL_ERROR)

include(ExternalProject)

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

add_library(mongo INTERFACE)
target_include_directories(mongo
  INTERFACE
  ${MONGO_INSTALL_DIR}/install/include/bsoncxx/v_noabi
  ${MONGO_INSTALL_DIR}/install/include/mongocxx/v_noabi
)
target_link_libraries(mongo
  INTERFACE 
  ${MONGO_INSTALL_DIR}/install/lib/libmongocxx${LIB_EXT}
  ${MONGO_INSTALL_DIR}/install/lib/libbsoncxx${LIB_EXT}
)