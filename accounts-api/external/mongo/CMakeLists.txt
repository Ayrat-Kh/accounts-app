cmake_minimum_required(VERSION VERSION 3.5 FATAL_ERROR)

include(ExternalProject)

# mongo prefetch
ExternalProject_Add(
  mongo_content
  GIT_REPOSITORY https://github.com/mongodb/mongo-cxx-driver
  GIT_TAG r3.10.1
  CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release -DENABLE_BSONCXX_POLY_USE_IMPLS=1 -DENABLE_TESTS=OFF
)
ExternalProject_Get_property(mongo_content BINARY_DIR)
set(MONGO_INSTALL_DIR ${BINARY_DIR})

add_library(mongoLibrary INTERFACE)
add_dependencies(mongoLibrary mongo_content)
target_include_directories(mongoLibrary
  INTERFACE
  ${MONGO_INSTALL_DIR}/install/include/bsoncxx/v_noabi
  ${MONGO_INSTALL_DIR}/install/include/mongocxx/v_noabi
  mongo_content
)
target_link_libraries(mongoLibrary
  INTERFACE 
  ${MONGO_INSTALL_DIR}/install/lib/libmongocxx${LIB_EXT}
  ${MONGO_INSTALL_DIR}/install/lib/libbsoncxx${LIB_EXT} 
)