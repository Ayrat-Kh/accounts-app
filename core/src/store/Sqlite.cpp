#include "store/Sqlite.hpp"

#include <sqlite3.h>

#include <stdexcept>

namespace receipts::store {

namespace {

[[noreturn]] void throwSqliteError(sqlite3 *database, const std::string &context) {
    throw std::runtime_error("sqlite " + context + ": " + sqlite3_errmsg(database));
}

} // namespace

Statement::Statement(sqlite3_stmt *statement) : statement_(statement, &sqlite3_finalize) {}

void Statement::bind(int index, const std::string &value) {
    sqlite3_bind_text(statement_.get(), index, value.data(), static_cast<int>(value.size()), SQLITE_TRANSIENT);
}

void Statement::bind(int index, std::int64_t value) {
    sqlite3_bind_int64(statement_.get(), index, value);
}

void Statement::bind(int index, double value) {
    sqlite3_bind_double(statement_.get(), index, value);
}

void Statement::bind(int index, std::nullopt_t) {
    sqlite3_bind_null(statement_.get(), index);
}

bool Statement::step() {
    const int result = sqlite3_step(statement_.get());
    if (result == SQLITE_ROW) {
        return true;
    }
    if (result == SQLITE_DONE) {
        return false;
    }
    throwSqliteError(sqlite3_db_handle(statement_.get()), "step");
}

void Statement::run() {
    while (step()) {
    }
}

bool Statement::isNull(int column) const {
    return sqlite3_column_type(statement_.get(), column) == SQLITE_NULL;
}

std::string Statement::text(int column) const {
    const auto *value = reinterpret_cast<const char *>(sqlite3_column_text(statement_.get(), column));
    return value ? std::string(value, static_cast<std::size_t>(sqlite3_column_bytes(statement_.get(), column)))
                 : std::string{};
}

std::optional<std::string> Statement::optionalText(int column) const {
    return isNull(column) ? std::nullopt : std::optional(text(column));
}

std::int64_t Statement::int64(int column) const {
    return sqlite3_column_int64(statement_.get(), column);
}

std::optional<double> Statement::optionalDouble(int column) const {
    return isNull(column) ? std::nullopt : std::optional(sqlite3_column_double(statement_.get(), column));
}

Result<std::unique_ptr<Database>> Database::open(const std::string &path) {
    sqlite3 *database = nullptr;
    // FULLMUTEX: one connection, safely shared by the host's background threads.
    const int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX;
    if (sqlite3_open_v2(path.c_str(), &database, flags, nullptr) != SQLITE_OK) {
        std::string message = database ? sqlite3_errmsg(database) : "out of memory";
        sqlite3_close(database);
        return fail(ErrorCode::Internal, "cannot open " + path + ": " + message);
    }
    sqlite3_busy_timeout(database, 5000);
    return std::unique_ptr<Database>(new Database(database));
}

Database::~Database() {
    sqlite3_close(database_);
}

void Database::execute(const std::string &sql) {
    char *error = nullptr;
    if (sqlite3_exec(database_, sql.c_str(), nullptr, nullptr, &error) != SQLITE_OK) {
        const std::string message = error ? error : "unknown error";
        sqlite3_free(error);
        throw std::runtime_error("sqlite exec: " + message);
    }
}

Statement Database::prepare(const std::string &sql) {
    sqlite3_stmt *statement = nullptr;
    if (sqlite3_prepare_v2(database_, sql.c_str(), -1, &statement, nullptr) != SQLITE_OK) {
        throwSqliteError(database_, "prepare");
    }
    return Statement(statement);
}

int Database::changes() const {
    return sqlite3_changes(database_);
}

} // namespace receipts::store
