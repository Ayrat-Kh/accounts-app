// Integration test against a real Garage (scripts/dev-up.sh). Skipped when
// ACCOUNTS_TEST_S3_ENDPOINT is not set.

#include "storage/S3ObjectStore.hpp"
#include "support/TestSupport.hpp"

#include <catch2/catch_test_macros.hpp>

#include <cstdlib>

using namespace accounts;
using testing::runSync;

namespace {

std::optional<S3Config> testS3Config() {
    auto env = [](const char *name) -> std::string {
        const char *value = std::getenv(name);
        return value ? value : "";
    };
    S3Config config{.endpoint = env("ACCOUNTS_TEST_S3_ENDPOINT"),
                    .region = "garage",
                    .bucket = env("ACCOUNTS_TEST_S3_BUCKET"),
                    .accessKey = env("ACCOUNTS_TEST_S3_ACCESS_KEY"),
                    .secretKey = env("ACCOUNTS_TEST_S3_SECRET_KEY")};
    if (config.endpoint.empty()) {
        return std::nullopt;
    }
    return config;
}

} // namespace

TEST_CASE("S3ObjectStore stores, range-reads and deletes objects in Garage") {
    const auto config = testS3Config();
    if (!config) {
        SKIP("ACCOUNTS_TEST_S3_ENDPOINT is not set (run scripts/dev-up.sh)");
    }
    storage::AwsSdkScope awsSdk;
    BlockingExecutor blocking(2);
    storage::S3ObjectStore store(*config, blocking);
    const std::string key = "tests/" + testing::uniqueSuffix();

    REQUIRE(runSync(store.put(key, "hello, receipts", "text/plain")));

    const auto whole = runSync(store.get(key, std::nullopt)).value();
    REQUIRE(whole);
    CHECK(whole->bytes == "hello, receipts");
    CHECK(whole->totalSize == 15);

    const auto piece = runSync(store.get(key, storage::ByteRange{7, 14})).value();
    REQUIRE(piece);
    CHECK(piece->bytes == "receipts");
    CHECK(piece->totalSize == 15);

    REQUIRE(runSync(store.remove(key)));
    CHECK_FALSE(runSync(store.get(key, std::nullopt)).value());
}
