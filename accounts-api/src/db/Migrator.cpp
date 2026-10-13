#include "db/Migrator.hpp"

#include "db/PgConnection.hpp"

#include <spdlog/spdlog.h>

#include <set>

namespace accounts::db {

using boost::asio::awaitable;

namespace {

// Any constant works; it only has to be the same in every replica.
constexpr std::string_view lockMigrations = "SELECT pg_advisory_lock(727274)";
constexpr std::string_view unlockMigrations = "SELECT pg_advisory_unlock(727274)";

awaitable<Result<std::set<std::string>>> appliedMigrationNames(PgConnection &connection) {
    auto created = co_await connection.executeScript(
        "CREATE TABLE IF NOT EXISTS schema_migrations ("
        "  name text PRIMARY KEY,"
        "  applied_at timestamptz NOT NULL DEFAULT now())");
    if (!created) {
        co_return std::unexpected(created.error());
    }
    auto result = co_await connection.execute("SELECT name FROM schema_migrations", {});
    if (!result) {
        co_return std::unexpected(result.error());
    }
    std::set<std::string> names;
    for (const PgRow row : result->rows()) {
        names.insert(row.text("name"));
    }
    co_return names;
}

awaitable<Result<void>> applyMigration(PgConnection &connection, const Migration &migration) {
    if (auto began = co_await connection.executeScript("BEGIN"); !began) {
        co_return began;
    }
    auto applied = co_await connection.executeScript(migration.sql);
    if (applied) {
        auto recorded =
            co_await connection.execute("INSERT INTO schema_migrations (name) VALUES ($1)", {migration.name});
        if (!recorded) {
            applied = std::unexpected(recorded.error());
        }
    }
    if (!applied) {
        co_await connection.executeScript("ROLLBACK");
        co_return fail(ErrorCode::Internal, "migration " + migration.name + " failed: " + applied.error().message);
    }
    co_return co_await connection.executeScript("COMMIT");
}

awaitable<Result<void>> applyPendingMigrations(PgConnection &connection, const std::vector<Migration> &migrations) {
    auto applied = co_await appliedMigrationNames(connection);
    if (!applied) {
        co_return std::unexpected(applied.error());
    }
    for (const Migration &migration : migrations) {
        if (applied->contains(migration.name)) {
            continue;
        }
        spdlog::info("applying migration {}", migration.name);
        if (auto result = co_await applyMigration(connection, migration); !result) {
            co_return result;
        }
    }
    co_return Result<void>{};
}

} // namespace

awaitable<Result<void>> runMigrations(std::string connectionString, const std::vector<Migration> &migrations) {
    auto connection = co_await PgConnection::connect(std::move(connectionString));
    if (!connection) {
        co_return std::unexpected(connection.error());
    }
    PgConnection &db = **connection;

    if (auto locked = co_await db.executeScript(std::string(lockMigrations)); !locked) {
        co_return locked;
    }
    auto result = co_await applyPendingMigrations(db, migrations);
    // Closing the connection would release the lock too; unlocking explicitly
    // just makes the intent obvious.
    co_await db.executeScript(std::string(unlockMigrations));
    co_return result;
}

} // namespace accounts::db
