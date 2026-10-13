#include "auth/BearerToken.hpp"

#include <algorithm>
#include <cctype>

namespace accounts::auth {

std::optional<std::string_view> parseBearerToken(std::string_view authorizationHeader) {
    constexpr std::string_view scheme = "bearer ";
    if (authorizationHeader.size() <= scheme.size()) {
        return std::nullopt;
    }
    const bool hasScheme = std::equal(scheme.begin(), scheme.end(), authorizationHeader.begin(),
                                      [](char expected, char actual) {
                                          return expected == std::tolower(static_cast<unsigned char>(actual));
                                      });
    if (!hasScheme) {
        return std::nullopt;
    }
    std::string_view token = authorizationHeader.substr(scheme.size());
    while (!token.empty() && token.front() == ' ') {
        token.remove_prefix(1);
    }
    if (token.empty() || token.find(' ') != std::string_view::npos) {
        return std::nullopt;
    }
    return token;
}

} // namespace accounts::auth
