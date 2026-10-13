cmake_minimum_required(VERSION 3.25)

# Gradle configures this once per ABI. Pick the matching vcpkg triplet and
# let vcpkg build the core's dependencies with the NDK's toolchain, before
# project() runs the toolchain file.
if(ANDROID_ABI STREQUAL "arm64-v8a")
    set(VCPKG_TARGET_TRIPLET arm64-android)
elseif(ANDROID_ABI STREQUAL "x86_64")
    set(VCPKG_TARGET_TRIPLET x64-android)
else()
    message(FATAL_ERROR "unsupported ABI: ${ANDROID_ABI}")
endif()
set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE "${ANDROID_NDK}/build/cmake/android.toolchain.cmake")
set(VCPKG_MANIFEST_DIR "${RECEIPTS_REPOSITORY_ROOT}/core")
# Shared by every variant and ABI, so Debug and Release don't build the
# dependencies twice.
set(VCPKG_INSTALLED_DIR "${RECEIPTS_REPOSITORY_ROOT}/core/build/android-vcpkg")

project(receipts_jni LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(RECEIPTS_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(RECEIPTS_BUILD_CLI OFF CACHE BOOL "" FORCE)
add_subdirectory("${RECEIPTS_REPOSITORY_ROOT}/core" core)

find_package(nlohmann_json CONFIG REQUIRED)

add_library(receipts_jni SHARED
    JniSupport.cpp
    JsonMapping.cpp
    KotlinPorts.cpp
    ReceiptsCoreJni.cpp)
target_compile_options(receipts_jni PRIVATE -Wall -Wextra)
target_link_libraries(receipts_jni PRIVATE receipts_core nlohmann_json::nlohmann_json log)
