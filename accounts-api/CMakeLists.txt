cmake_minimum_required(VERSION 3.25)

project(accounts_api LANGUAGES CXX)

# C++23 for std::expected; GCC 14 (dev image) also gives us <format> and
# std::chrono::parse.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Dependencies (pinned by vcpkg.json's builtin-baseline) ------------------
find_package(Boost CONFIG REQUIRED COMPONENTS asio beast json)
# uSockets comes from vcpkg-overlays/usockets, built with the Asio backend.
find_package(unofficial-usockets CONFIG REQUIRED)
find_package(unofficial-uwebsockets CONFIG REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(AWSSDK REQUIRED COMPONENTS s3)
find_package(jwt-cpp CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(tomlplusplus CONFIG REQUIRED)
find_package(Threads REQUIRED)

# --- Migrations are compiled into the binary --------------------------------
# The distroless runtime image holds nothing but the executable, so the SQL
# files under migrations/ are turned into a generated .cpp at build time.
include(cmake/EmbedMigrations.cmake)
embed_migrations(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated/EmbeddedMigrations.cpp
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/migrations)

# --- accounts_core: everything except main.cpp (shared with the tests) -------
add_library(accounts_core STATIC
    src/assets/AssetRoutes.cpp
    src/auth/AccessTokenService.cpp
    src/auth/AuthRoutes.cpp
    src/auth/AuthService.cpp
    src/auth/BearerToken.cpp
    src/auth/GoogleIdentityVerifier.cpp
    src/auth/RefreshTokenRepository.cpp
    src/auth/UserRepository.cpp
    src/config/Config.cpp
    src/db/Migrator.cpp
    src/db/PgConnection.cpp
    src/db/PgPool.cpp
    src/db/PgResult.cpp
    src/health/HealthRoutes.cpp
    src/http/JsonFields.cpp
    src/http/Response.cpp
    src/http/Router.cpp
    src/http/Server.cpp
    src/net/HttpsClient.cpp
    src/receipts/Receipt.cpp
    src/receipts/ReceiptJson.cpp
    src/receipts/ReceiptRepository.cpp
    src/receipts/ReceiptRoutes.cpp
    src/receipts/ReceiptService.cpp
    src/storage/InMemoryObjectStore.cpp
    src/storage/S3ObjectStore.cpp
    src/sync/ObsidianSyncWorker.cpp
    src/util/Crypto.cpp
    src/util/Files.cpp
    src/util/Time.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/generated/EmbeddedMigrations.cpp
)
target_include_directories(accounts_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_options(accounts_core PRIVATE -Wall -Wextra -Wpedantic)
target_compile_definitions(accounts_core PUBLIC
    BOOST_ASIO_NO_DEPRECATED
    # jwt-cpp speaks Boost.JSON here (jwt-cpp/traits/boost-json), so its
    # bundled picojson is never compiled.
    JWT_DISABLE_PICOJSON)
target_link_libraries(accounts_core PUBLIC
    Boost::asio
    Boost::beast
    Boost::json
    unofficial::uwebsockets::uwebsockets
    unofficial::usockets::usockets
    PostgreSQL::PostgreSQL
    ${AWSSDK_LINK_LIBRARIES}
    jwt-cpp::jwt-cpp
    OpenSSL::SSL
    OpenSSL::Crypto
    spdlog::spdlog
    tomlplusplus::tomlplusplus
    Threads::Threads)

# --- accounts-api: the executable --------------------------------------------
add_executable(accounts-api src/main.cpp)
target_link_libraries(accounts-api PRIVATE accounts_core)

# --- Tests -------------------------------------------------------------------
option(ACCOUNTS_BUILD_TESTS "Build the Catch2 test suite" ON)
if(ACCOUNTS_BUILD_TESTS)
    enable_testing()
    find_package(Catch2 3 CONFIG REQUIRED)
    add_executable(accounts_tests
        tests/AccessTokenServiceTest.cpp
        tests/BearerTokenTest.cpp
        tests/CursorTest.cpp
        tests/GoogleTokenInfoTest.cpp
        tests/HttpApiTest.cpp
        tests/JsonFieldsTest.cpp
        tests/ReceiptRepositoryTest.cpp
        tests/ReceiptValidationTest.cpp
        tests/RouterTest.cpp
        tests/S3ObjectStoreTest.cpp
        tests/TimeTest.cpp
        tests/support/TestDatabase.cpp
        tests/support/TestHttpClient.cpp
        tests/support/TestSupport.cpp)
    target_include_directories(accounts_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests)
    target_link_libraries(accounts_tests PRIVATE accounts_core Catch2::Catch2WithMain)
    include(Catch)
    catch_discover_tests(accounts_tests)
endif()
