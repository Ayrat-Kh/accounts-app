#include "util/Text.hpp"

#include <algorithm>
#include <cctype>

namespace receipts::text {

namespace {

// Appends the lower-case form of the UTF-8 code point starting at text[i]
// and returns its byte length.
std::size_t appendLowerCodePoint(std::string_view text, std::size_t i, std::string &out) {
    const auto byte = static_cast<unsigned char>(text[i]);
    if (byte < 0x80) {
        out.push_back(static_cast<char>(std::tolower(byte)));
        return 1;
    }
    if (i + 1 < text.size() && (byte == 0xC3 || byte == 0xD0)) {
        const auto next = static_cast<unsigned char>(text[i + 1]);
        if (byte == 0xC3 && next >= 0x80 && next <= 0x9E && next != 0x97) { // À..Þ except ×
            out.push_back(static_cast<char>(0xC3));
            out.push_back(static_cast<char>(next + 0x20));
            return 2;
        }
        if (byte == 0xD0 && next >= 0x90 && next <= 0x9F) { // А..П -> а..п
            out.push_back(static_cast<char>(0xD0));
            out.push_back(static_cast<char>(next + 0x20));
            return 2;
        }
        if (byte == 0xD0 && next >= 0xA0 && next <= 0xAF) { // Р..Я -> р..я
            out.push_back(static_cast<char>(0xD1));
            out.push_back(static_cast<char>(next - 0x20));
            return 2;
        }
        if (byte == 0xD0 && next == 0x81) { // Ё -> ё
            out += "\xD1\x91";
            return 2;
        }
    }
    out.push_back(text[i]);
    return 1;
}

bool isNumberCharacter(char c) {
    return std::isdigit(static_cast<unsigned char>(c)) || c == '.' || c == ',' || c == '-' || c == '\'' || c == '%' ||
           c == '$' || c == '*';
}

} // namespace

std::string toLower(std::string_view utf8) {
    std::string lower;
    lower.reserve(utf8.size());
    for (std::size_t i = 0; i < utf8.size();) {
        i += appendLowerCodePoint(utf8, i, lower);
    }
    return lower;
}

std::string trim(std::string_view text) {
    const auto isSpace = [](char c) { return std::isspace(static_cast<unsigned char>(c)) != 0; };
    while (!text.empty() && isSpace(text.front())) {
        text.remove_prefix(1);
    }
    while (!text.empty() && isSpace(text.back())) {
        text.remove_suffix(1);
    }
    return std::string(text);
}

std::vector<std::string> splitLines(std::string_view text) {
    std::vector<std::string> lines;
    std::size_t start = 0;
    while (start <= text.size()) {
        const auto newline = text.find('\n', start);
        const auto end = newline == std::string_view::npos ? text.size() : newline;
        if (std::string line = trim(text.substr(start, end - start)); !line.empty()) {
            lines.push_back(std::move(line));
        }
        if (newline == std::string_view::npos) {
            break;
        }
        start = newline + 1;
    }
    return lines;
}

bool containsDigit(std::string_view text) {
    return std::ranges::any_of(text, [](char c) { return std::isdigit(static_cast<unsigned char>(c)) != 0; });
}

std::string withoutNumbers(std::string_view line) {
    std::string words;
    bool lastWasSpace = true;
    for (char c : line) {
        const bool blank = isNumberCharacter(c) || std::isspace(static_cast<unsigned char>(c));
        if (blank) {
            if (!lastWasSpace) {
                words.push_back(' ');
            }
            lastWasSpace = true;
        } else {
            words.push_back(c);
            lastWasSpace = false;
        }
    }
    return trim(words);
}

} // namespace receipts::text
