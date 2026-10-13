#include "util/Time.hpp"

#include <format>
#include <sstream>

namespace accounts {

Timestamp now() {
    return std::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now());
}

std::string formatRfc3339(Timestamp timestamp) {
    // %T prints seconds with the time point's precision, i.e. "SS.mmm".
    return std::format("{:%FT%T}Z", timestamp);
}

std::optional<Timestamp> parseRfc3339(std::string_view text) {
    std::istringstream input{std::string(text)};
    std::chrono::sys_time<std::chrono::milliseconds> parsed;
    // %Ez accepts "+01:00"; a literal 'Z' is handled by the first format.
    for (const char *format : {"%FT%TZ", "%FT%T%Ez"}) {
        input.clear();
        input.seekg(0);
        input >> std::chrono::parse(format, parsed);
        if (input && input.peek() == std::char_traits<char>::eof()) {
            return parsed;
        }
    }
    return std::nullopt;
}

} // namespace accounts
