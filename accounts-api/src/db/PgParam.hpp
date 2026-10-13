#pragma once

// Query parameters are sent to Postgres as text (libpq's default), and a
// missing optional becomes SQL NULL. toPgParam() is overloaded for each C++
// type that the repositories pass, which lets them write
// `pool.query(sql, userId, amountMinor, merchant)` with no manual conversions.

#include "util/Time.hpp"

#include <cstdint>
#include <format>
#include <optional>
#include <string>
#include <string_view>

namespace accounts::db {

using PgParam = std::optional<std::string>;

inline PgParam toPgParam(std::string_view value) { return std::string(value); }
inline PgParam toPgParam(const std::string &value) { return value; }
inline PgParam toPgParam(const char *value) { return std::string(value); }
inline PgParam toPgParam(std::int64_t value) { return std::to_string(value); }
inline PgParam toPgParam(int value) { return std::to_string(value); }
inline PgParam toPgParam(double value) { return std::format("{}", value); }  // shortest exact form
inline PgParam toPgParam(bool value) { return value ? "true" : "false"; }
inline PgParam toPgParam(Timestamp value) { return formatRfc3339(value); }
inline PgParam toPgParam(std::nullopt_t) { return std::nullopt; }

template <class T>
PgParam toPgParam(const std::optional<T> &value) {
    return value ? toPgParam(*value) : std::nullopt;
}

} // namespace accounts::db
