#pragma once

// Small text helpers. Receipts in this app are English, German or Russian, so
// lower-casing handles ASCII plus Latin-1 umlauts and Cyrillic in UTF-8;
// that is enough for keyword matching and avoids an ICU dependency.

#include <string>
#include <string_view>
#include <vector>

namespace receipts::text {

std::string toLower(std::string_view utf8);
std::string trim(std::string_view text);
std::vector<std::string> splitLines(std::string_view text);
bool containsDigit(std::string_view text);

// Replaces every run of digits, separators and currency marks with one space:
// "TOTAL   12,34 EUR" -> "TOTAL EUR". The words around a number say what the
// number is; the digits themselves only add noise to an embedding.
std::string withoutNumbers(std::string_view line);

} // namespace receipts::text
