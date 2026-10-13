#pragma once

// RFC 3339 timestamps without std::chrono::parse (missing from Android's
// libc++). Civil-date math follows Howard Hinnant's well-known algorithms.

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace receipts::time {

std::int64_t nowUnixMillis();

// 1760347800000 -> "2025-10-13T09:30:00.000Z"
std::string formatRfc3339(std::int64_t unixMillis);

// "YYYY-MM-DD" -> noon UTC that day, as RFC 3339. Receipts carry a date but
// rarely a time; noon keeps the date the same in every time zone around UTC.
std::optional<std::string> dateToRfc3339Noon(std::string_view isoDate);

// Days since 1970-01-01 for a valid civil date, nullopt otherwise.
std::optional<std::int64_t> daysFromCivil(int year, int month, int day);

} // namespace receipts::time
