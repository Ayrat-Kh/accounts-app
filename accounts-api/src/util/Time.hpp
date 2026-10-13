#pragma once

// Timestamps travel as RFC 3339 strings in UTC ("2026-10-13T09:30:00.000Z"),
// both in the JSON API and out of Postgres (see the rfc3339() SQL function in
// migrations/001_init.sql). Inside the program they are time points with
// millisecond precision.

#include <chrono>
#include <optional>
#include <string>
#include <string_view>

namespace accounts {

using Timestamp = std::chrono::sys_time<std::chrono::milliseconds>;

Timestamp now();

std::string formatRfc3339(Timestamp timestamp);

// Accepts "YYYY-MM-DDTHH:MM:SS[.fff](Z|±HH:MM)". Returns nullopt for anything else.
std::optional<Timestamp> parseRfc3339(std::string_view text);

} // namespace accounts
