cmake_minimum_required(VERSION 3.25)
project(uSockets C CXX)

option(WITH_OPENSSL "Enables OpenSSL 1.1+ support")
option(WITH_ASIO "Use boost::asio as the event loop")

file(GLOB C_SOURCES src/*.c src/eventing/*.c)
add_library(uSockets ${C_SOURCES})
set_target_properties(uSockets PROPERTIES EXPORT_NAME usockets)
target_include_directories(uSockets
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>"
        "$<INSTALL_INTERFACE:include>"
)

if(WITH_OPENSSL)
    file(GLOB CRYPTO_SOURCES src/crypto/*.c*)
    target_sources(uSockets PRIVATE ${CRYPTO_SOURCES})
    target_compile_features(uSockets PRIVATE cxx_std_17)
    find_package(OpenSSL REQUIRED)
    target_link_libraries(uSockets PRIVATE OpenSSL::SSL OpenSSL::Crypto)
    target_compile_definitions(uSockets PRIVATE LIBUS_USE_OPENSSL)
else()
    target_compile_definitions(uSockets PRIVATE LIBUS_NO_SSL)
endif()

if(WITH_ASIO)
    target_sources(uSockets PRIVATE src/eventing/asio.cpp)
    target_compile_features(uSockets PRIVATE cxx_std_17)
    find_package(boost_asio CONFIG REQUIRED)
    target_link_libraries(uSockets PUBLIC Boost::asio)
    # PUBLIC: libusockets.h picks the backend from this define, and the
    # library and its users must agree on it.
    target_compile_definitions(uSockets PUBLIC LIBUS_USE_ASIO)
endif()

install(TARGETS uSockets
    EXPORT unofficial-usockets-targets
    ARCHIVE DESTINATION lib
)
install(EXPORT unofficial-usockets-targets
    NAMESPACE unofficial::usockets::
    DESTINATION share/unofficial-usockets
)
configure_file("unofficial-usockets-config.cmake" "${CMAKE_CURRENT_BINARY_DIR}/unofficial-usockets-config.cmake" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/unofficial-usockets-config.cmake"
    DESTINATION share/unofficial-usockets
)
install(FILES src/libusockets.h DESTINATION include)
