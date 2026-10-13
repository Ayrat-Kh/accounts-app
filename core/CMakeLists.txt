cmake_minimum_required(VERSION 3.25)

# receipts-core: the platform-independent logic of the receipts app.
# Built for Android (through Gradle's externalNativeBuild), macOS and Linux.
project(receipts_core LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)  # linked into a shared library on Android

option(RECEIPTS_BUILD_TESTS "Build the Catch2 test suite" ON)
option(RECEIPTS_BUILD_CLI "Build the receipts-cli tool" ON)

# --- Dependencies (pinned by vcpkg.json's builtin-baseline + vcpkg-overlays) --
find_package(Tesseract CONFIG REQUIRED)
find_package(Leptonica CONFIG REQUIRED)
find_package(llama CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_library(receipts_core STATIC
    src/api/ApiClient.cpp
    src/api/ApiJson.cpp
    src/app/ReceiptsService.cpp
    src/app/SyncEngine.cpp
    src/extract/AmountParser.cpp
    src/extract/EmbeddingLineScorer.cpp
    src/extract/KeywordLineScorer.cpp
    src/extract/LlmTotalExtractor.cpp
    src/extract/PriceExtractor.cpp
    src/extract/ReceiptFields.cpp
    src/inference/LlamaEmbeddingEngine.cpp
    src/inference/OpenAiCompatibleEngine.cpp
    src/models/ModelDownloader.cpp
    src/net/CurlHttpTransport.cpp
    src/ocr/TesseractOcrEngine.cpp
    src/store/LocalReceiptStore.cpp
    src/store/Sqlite.cpp
    src/util/Files.cpp
    src/util/Text.cpp
    src/util/Time.cpp
)
target_include_directories(receipts_core
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include   # the public API (what hosts use)
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)      # internals
target_compile_options(receipts_core PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(receipts_core PRIVATE
    Tesseract::libtesseract
    leptonica
    llama
    CURL::libcurl
    OpenSSL::Crypto
    unofficial::sqlite3::sqlite3
    nlohmann_json::nlohmann_json)

# Internals, for the CLI and tests that exercise pieces below the public API.
add_library(receipts_core_internals INTERFACE)
target_include_directories(receipts_core_internals INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(receipts_core_internals INTERFACE receipts_core nlohmann_json::nlohmann_json)

if(RECEIPTS_BUILD_CLI)
    add_executable(receipts-cli tools/receipts-cli/main.cpp)
    target_link_libraries(receipts-cli PRIVATE receipts_core_internals)
endif()

if(RECEIPTS_BUILD_TESTS)
    enable_testing()
    find_package(Catch2 3 CONFIG REQUIRED)
    add_executable(receipts_core_tests
        tests/AmountParserTest.cpp
        tests/ApiClientTest.cpp
        tests/LlmTotalExtractorTest.cpp
        tests/LocalReceiptStoreTest.cpp
        tests/ModelDownloaderTest.cpp
        tests/OpenAiCompatibleEngineTest.cpp
        tests/PriceExtractorTest.cpp
        tests/ReceiptFieldsTest.cpp
        tests/ScanAccuracyTest.cpp
        tests/SyncEngineTest.cpp
        tests/TextAndTimeTest.cpp)
    target_include_directories(receipts_core_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests)
    target_link_libraries(receipts_core_tests PRIVATE receipts_core_internals Catch2::Catch2WithMain)
    target_compile_definitions(receipts_core_tests PRIVATE
        RECEIPTS_TESTDATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/testdata")
    include(Catch)
    catch_discover_tests(receipts_core_tests)
endif()
