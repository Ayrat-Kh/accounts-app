#pragma once

// The receipt fields other than the total: purchase date and merchant.

#include <optional>
#include <string>
#include <vector>

namespace receipts::extract {

// The first valid date on the receipt as "YYYY-MM-DD". Understands
// "2026-10-13", "13.10.2026", "13/10/26". Slash dates are read day-first
// (European) unless that is impossible ("10/13/2026" is October 13th).
std::optional<std::string> findPurchaseDate(const std::vector<std::string> &lines);

// Shops print their name first. The first of the top three lines that is
// mostly letters is taken as the merchant.
std::optional<std::string> guessMerchant(const std::vector<std::string> &lines);

} // namespace receipts::extract
