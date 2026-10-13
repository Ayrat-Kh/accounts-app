#pragma once

// RAII ownership of a libpq PGresult plus a small, typed row reader:
//
//     for (const PgRow row : result.rows()) {
//         receipts.push_back({.id = row.text("id"), .amountMinor = row.int64("amount_minor")});
//     }
//
// Columns are looked up by name, so a SELECT can reorder columns freely.
// All values arrive in libpq's text format and are converted here.

#include <libpq-fe.h>

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace accounts::db {

class PgRow {
public:
    PgRow(const PGresult *result, int row) : result_(result), row_(row) {}

    bool isNull(std::string_view column) const;
    std::string text(std::string_view column) const;
    std::optional<std::string> optionalText(std::string_view column) const;
    std::int64_t int64(std::string_view column) const;
    std::optional<std::int64_t> optionalInt64(std::string_view column) const;
    std::optional<double> optionalDouble(std::string_view column) const;

private:
    int columnIndex(std::string_view column) const; // throws std::logic_error for an unknown column

    const PGresult *result_;
    int row_;
};

class PgResult {
public:
    explicit PgResult(PGresult *result) : result_(result, &PQclear) {}

    int rowCount() const { return PQntuples(result_.get()); }
    bool empty() const { return rowCount() == 0; }
    PgRow row(int index) const { return {result_.get(), index}; }
    PgRow firstRow() const { return row(0); }

    // Enables `for (const PgRow row : result.rows())`.
    class RowRange {
    public:
        class Iterator {
        public:
            Iterator(const PGresult *result, int row) : result_(result), row_(row) {}
            PgRow operator*() const { return {result_, row_}; }
            Iterator &operator++() {
                ++row_;
                return *this;
            }
            bool operator!=(const Iterator &other) const { return row_ != other.row_; }

        private:
            const PGresult *result_;
            int row_;
        };

        explicit RowRange(const PGresult *result) : result_(result) {}
        Iterator begin() const { return {result_, 0}; }
        Iterator end() const { return {result_, PQntuples(result_)}; }

    private:
        const PGresult *result_;
    };

    RowRange rows() const { return RowRange(result_.get()); }

private:
    std::unique_ptr<PGresult, decltype(&PQclear)> result_;
};

} // namespace accounts::db
