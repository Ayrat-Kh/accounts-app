#include "extract/AmountParser.hpp"

#include "util/Text.hpp"

#include <array>
#include <regex>

namespace receipts::extract {

namespace {

// (sign)(integer part, optionally grouped in thousands)(decimal separator)(two digits),
// not followed by a digit or by another separator and digit, so the "13.10"
// in the date "13.10.2026" is not taken for a price.
// Group 1: an optional minus. Group 2: the integer part. Group 3: the cents.
const std::regex amountPattern(R"((-?)\s?(\d{1,3}(?:[ .,']\d{3})+|\d+)[.,](\d{2})(?![.,]?\d))");

struct CurrencyMark {
    std::string_view mark;  // matched against the lower-cased line
    std::string_view isoCode;
};

constexpr std::array currencyMarks = {
    CurrencyMark{"€", "EUR"},   CurrencyMark{"eur", "EUR"}, CurrencyMark{"$", "USD"},
    CurrencyMark{"usd", "USD"}, CurrencyMark{"£", "GBP"},   CurrencyMark{"gbp", "GBP"},
    CurrencyMark{"₽", "RUB"},   CurrencyMark{"rub", "RUB"}, CurrencyMark{"руб", "RUB"},
    CurrencyMark{"zł", "PLN"},  CurrencyMark{"pln", "PLN"}, CurrencyMark{"chf", "CHF"},
    CurrencyMark{"czk", "CZK"}, CurrencyMark{"kč", "CZK"},
};

std::int64_t integerFromDigits(std::string_view text) {
    std::int64_t value = 0;
    for (char c : text) {
        if (c >= '0' && c <= '9') {
            value = value * 10 + (c - '0');  // separators are simply skipped
        }
    }
    return value;
}

} // namespace

std::vector<ParsedAmount> findAmounts(std::string_view line) {
    std::vector<ParsedAmount> amounts;
    const std::string text(line);
    for (std::sregex_iterator match(text.begin(), text.end(), amountPattern), end; match != end; ++match) {
        const bool isNegative = (*match)[1].length() > 0;
        if (isNegative) {
            continue;
        }
        const std::int64_t whole = integerFromDigits((*match)[2].str());
        const std::int64_t cents = integerFromDigits((*match)[3].str());
        amounts.push_back({whole * 100 + cents, text::trim(match->str())});
    }
    return amounts;
}

std::optional<std::string> findCurrency(std::string_view line) {
    const std::string lower = text::toLower(line);
    std::optional<std::string> found;
    std::size_t foundAt = std::string::npos;
    for (const CurrencyMark &currency : currencyMarks) {
        const auto position = lower.find(currency.mark);
        if (position < foundAt) {
            foundAt = position;
            found = std::string(currency.isoCode);
        }
    }
    return found;
}

} // namespace receipts::extract
