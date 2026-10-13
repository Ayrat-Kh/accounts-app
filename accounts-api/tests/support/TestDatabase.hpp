#pragma once

// Integration tests need a real Postgres. scripts/dev-up.sh starts one and
// writes ACCOUNTS_TEST_DATABASE_URL into scripts/.dev-env; CI sets it as well.
// Without it, database tests are skipped rather than failed, so the unit
// tests still run anywhere.

#include "auth/UserRepository.hpp"
#include "db/PgPool.hpp"

#include <memory>
#include <optional>
#include <string>

namespace accounts::testing {

// The connection string, or nullopt when no test database is configured.
std::optional<std::string> testDatabaseUrl();

// A pool on the test database, with all migrations applied (once per run).
// Calls Catch2's SKIP() when no test database is configured.
db::PgPool &testDatabase();

// Creates a user with a unique Google subject; every test gets fresh users,
// so tests need no cleanup and can run in any order.
auth::User createTestUser();

} // namespace accounts::testing
