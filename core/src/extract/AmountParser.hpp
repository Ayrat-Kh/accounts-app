#pragma once

// Finds money amounts and currency marks in one line of receipt text.
//
// Receipts print prices with exactly two decimals, in either convention:
//   "12,34"  "12.34"  "1.234,56"  "1,234.56"  "1 234,56"
// so a number counts as an amount only if it ends in a separator plus two
// digits. That skips quantities ("2 x"), weights ("0,512 kg"), dates and
// times, and article numbers. Negative amounts (discounts, refunds) are
// skipped too: a total is never negative.

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace receipts::extract {

struct ParsedAmount {
    std::int64_t amountMinor;
    std::string text; // as printed, e.g. "1.234,56"
};

std::vector<ParsedAmount> findAmounts(std::string_view line);

// ISO 4217 code for the first currency mark in the line ("€", "EUR", "руб", ...).
std::optional<std::string> findCurrency(std::string_view line);

} // namespace receipts::extract
