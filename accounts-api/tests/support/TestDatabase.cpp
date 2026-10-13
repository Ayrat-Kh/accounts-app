#include "support/TestDatabase.hpp"

#include "db/Migrator.hpp"
#include "support/TestSupport.hpp"

#include <catch2/catch_test_macros.hpp>

#include <cstdlib>

namespace accounts::testing {

std::optional<std::string> testDatabaseUrl() {
    const char *url = std::getenv("ACCOUNTS_TEST_DATABASE_URL");
    if (url == nullptr || *url == '\0') {
        return std::nullopt;
    }
    return std::string(url);
}

db::PgPool &testDatabase() {
    const auto url = testDatabaseUrl();
    if (!url) {
        SKIP("ACCOUNTS_TEST_DATABASE_URL is not set (run scripts/dev-up.sh)");
    }
    // Statics are destroyed in reverse order of construction. Touch the
    // io_context first so it outlives the pool, whose sockets it owns.
    testIoContext();
    static std::unique_ptr<db::PgPool> pool;
    if (!pool) {
        auto migrated = runSync(db::runMigrations(*url, db::embeddedMigrations()));
        REQUIRE(migrated.has_value());
        pool = std::make_unique<db::PgPool>(*url, 4);
    }
    return *pool;
}

auth::User createTestUser() {
    auth::UserRepository users(testDatabase());
    const std::string suffix = uniqueSuffix();
    auto user = runSync(users.upsertByGoogleSubject("google-" + suffix, suffix + "@example.com"));
    REQUIRE(user.has_value());
    return *user;
}

} // namespace accounts::testing
