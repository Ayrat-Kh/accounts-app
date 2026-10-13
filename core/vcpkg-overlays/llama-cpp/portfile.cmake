# Overlay of vcpkg's llama-cpp port, for two reasons:
#  1. EmbeddingGemma 2 (architecture "gemma-embedding2") needs llama.cpp
#     b11446 or newer (ggml-org/llama.cpp#30054, 2026-10-06); our vcpkg
#     baseline ships v0.4.1, which predates it.
#  2. It builds llama.cpp together with the ggml it bundles, the combination
#     upstream tests, instead of vcpkg's separately versioned ggml port.
# Only the libraries are built: no tools, server, examples or network code.
vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO ggml-org/llama.cpp
    REF b11476
    SHA512 d58ceaab8e4552ca663810bc204c11b636501e29312afd8e4ecfcf4d278bfc1703f2857c57dd7a8f5651d36f1ef926a40d96097d6f4768110739760eca7e68d1
    HEAD_REF master
)

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
    FEATURES
        metal GGML_METAL
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        ${FEATURE_OPTIONS}
        -DLLAMA_BUILD_IS_DEV=OFF
        -DLLAMA_BUILD_COMMON=OFF
        -DLLAMA_BUILD_TESTS=OFF
        -DLLAMA_BUILD_TOOLS=OFF
        -DLLAMA_BUILD_EXAMPLES=OFF
        -DLLAMA_BUILD_SERVER=OFF
        -DLLAMA_BUILD_APP=OFF
        -DLLAMA_OPENSSL=OFF
        -DGGML_NATIVE=OFF            # portable binaries: never tune for the build machine's CPU
        -DGGML_OPENMP=OFF            # ggml's own thread pool; no libomp to ship on Android
        -DGGML_METAL_EMBED_LIBRARY=ON
)
vcpkg_cmake_install()
vcpkg_cmake_config_fixup(PACKAGE_NAME llama CONFIG_PATH lib/cmake/llama DO_NOT_DELETE_PARENT_CONFIG_PATH)
vcpkg_cmake_config_fixup(PACKAGE_NAME ggml CONFIG_PATH lib/cmake/ggml)
vcpkg_fixup_pkgconfig()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include" "${CURRENT_PACKAGES_DIR}/debug/share")
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
