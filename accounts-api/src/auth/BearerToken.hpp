#pragma once

#include <optional>
#include <string_view>

namespace accounts::auth {

// Extracts the token from "Authorization: Bearer <token>". The scheme is
// case-insensitive (RFC 7235); anything else, including an empty token,
// yields nullopt. The old code called remove_prefix(7) on any header,
// which is undefined behaviour for headers shorter than 7 characters.
std::optional<std::string_view> parseBearerToken(std::string_view authorizationHeader);

} // namespace accounts::auth
