#pragma once

// A thin RAII wrapper over SQLite's C API: a connection and prepared
// statements with typed bind/read helpers.
//
//     auto statement = database.prepare("SELECT name FROM t WHERE id = ?1");
//     statement->bind(1, id);
//     while (statement->step()) { names.push_back(statement->text(0)); }

#include "receipts/Result.hpp"

#include <cstdint>
#include <memory>
#include <optional>
#include <string>

struct sqlite3;
struct sqlite3_stmt;

namespace receipts::store {

class Statement {
public:
    explicit Statement(sqlite3_stmt *statement);

    void bind(int index, const std::string &value);
    void bind(int index, std::int64_t value);
    void bind(int index, double value);
    void bind(int index, std::nullopt_t);
    template <class T>
    void bind(int index, const std::optional<T> &value) {
        value ? bind(index, *value) : bind(index, std::nullopt);
    }

    // Advances to the next row; false when done. Throws on SQL errors, which
    // only bugs (bad SQL, broken schema) cause here.
    bool step();
    // For statements without result rows.
    void run();

    bool isNull(int column) const;
    std::string text(int column) const;
    std::optional<std::string> optionalText(int column) const;
    std::int64_t int64(int column) const;
    std::optional<double> optionalDouble(int column) const;

private:
    std::unique_ptr<sqlite3_stmt, int (*)(sqlite3_stmt *)> statement_;
};

class Database {
public:
    static Result<std::unique_ptr<Database>> open(const std::string &path);
    ~Database();

    void execute(const std::string &sql); // several statements, no parameters
    Statement prepare(const std::string &sql);
    int changes() const; // rows changed by the last statement

private:
    explicit Database(sqlite3 *database) : database_(database) {}
    sqlite3 *database_;
};

} // namespace receipts::store
