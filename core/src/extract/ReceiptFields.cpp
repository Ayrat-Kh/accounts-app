#include "extract/ReceiptFields.hpp"

#include "util/Text.hpp"
#include "util/Time.hpp"

#include <cctype>
#include <cstdio>
#include <regex>

namespace receipts::extract {

namespace {

const std::regex isoDatePattern(R"((\d{4})-(\d{2})-(\d{2}))");
const std::regex dottedDatePattern(R"((\d{1,2})[./](\d{1,2})[./](\d{4}|\d{2})(?!\d))");

std::optional<std::string> formatIfValid(int year, int month, int day) {
    if (!time::daysFromCivil(year, month, day)) {
        return std::nullopt;
    }
    char buffer[11];
    std::snprintf(buffer, sizeof buffer, "%04d-%02d-%02d", year, month, day);
    return std::string(buffer);
}

std::optional<std::string> dateInLine(const std::string &line) {
    std::smatch match;
    if (std::regex_search(line, match, isoDatePattern)) {
        return formatIfValid(std::stoi(match[1]), std::stoi(match[2]), std::stoi(match[3]));
    }
    if (std::regex_search(line, match, dottedDatePattern)) {
        const int first = std::stoi(match[1]);
        const int second = std::stoi(match[2]);
        int year = std::stoi(match[3]);
        if (year < 100) {
            year += 2000;
        }
        if (auto dayFirst = formatIfValid(year, second, first)) {
            return dayFirst;
        }
        return formatIfValid(year, first, second); // month first, US style
    }
    return std::nullopt;
}

bool isMostlyLetters(const std::string &line) {
    int letters = 0;
    int others = 0;
    for (unsigned char c : line) {
        if (std::isspace(c)) {
            continue;
        }
        // Bytes >= 0x80 belong to UTF-8 letters (umlauts, Cyrillic) on a receipt.
        (std::isalpha(c) || c >= 0x80) ? ++letters : ++others;
    }
    return letters >= 3 && letters > 2 * others;
}

} // namespace

std::optional<std::string> findPurchaseDate(const std::vector<std::string> &lines) {
    for (const std::string &line : lines) {
        if (auto date = dateInLine(line)) {
            return date;
        }
    }
    return std::nullopt;
}

std::optional<std::string> guessMerchant(const std::vector<std::string> &lines) {
    constexpr std::size_t linesToConsider = 3;
    for (std::size_t i = 0; i < lines.size() && i < linesToConsider; ++i) {
        if (isMostlyLetters(lines[i])) {
            return text::trim(lines[i]);
        }
    }
    return std::nullopt;
}

} // namespace receipts::extract
