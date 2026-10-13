#pragma once

// Schema migrations: plain SQL files under migrations/, compiled into the
// binary (cmake/EmbedMigrations.cmake) and applied at startup.
//
// Each file runs once, inside a transaction, and is recorded in
// schema_migrations. An advisory lock serialises concurrent starts, so two
// replicas rolling out at the same moment cannot both migrate.

#include "util/Error.hpp"

#include <boost/asio/awaitable.hpp>

#include <string>
#include <vector>

namespace accounts::db {

struct Migration {
    std::string name; // file name without extension, e.g. "001_init"; defines the order
    std::string sql;
};

const std::vector<Migration> &embeddedMigrations();

boost::asio::awaitable<Result<void>> runMigrations(std::string connectionString,
                                                   const std::vector<Migration> &migrations);

} // namespace accounts::db
