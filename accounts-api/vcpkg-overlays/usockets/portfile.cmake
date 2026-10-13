# Overlay of vcpkg's usockets port. Two differences from upstream vcpkg:
#  1. Pinned to a uSockets commit whose Asio backend builds with Boost >= 1.87
#     (v0.8.8 still uses the removed deadline_timer / io_context::post).
#  2. An "asio" feature that compiles src/eventing/asio.cpp instead of epoll,
#     so uWebSockets runs on the same io_context as our coroutines.
vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO uNetworking/uSockets
    REF 2353808c2e605c4f38bd9f09261fff13ae2a58be
    SHA512 c4958318e4ca0e0985295a9b6677ed3e0c2d0b40dbb08538629fafaf157aeff522dcf87ce22ee11a07e235fbe00c7228682d98246d0f4e50b2951405687211b3
    HEAD_REF master
)
file(COPY "${CURRENT_PORT_DIR}/CMakeLists.txt" DESTINATION "${SOURCE_PATH}")
file(COPY "${CURRENT_PORT_DIR}/unofficial-usockets-config.cmake" DESTINATION "${SOURCE_PATH}")

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
    FEATURES
        ssl     WITH_OPENSSL
        asio    WITH_ASIO
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        ${FEATURE_OPTIONS}
)
vcpkg_cmake_install()
vcpkg_cmake_config_fixup(PACKAGE_NAME unofficial-usockets)
vcpkg_copy_pdbs()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
