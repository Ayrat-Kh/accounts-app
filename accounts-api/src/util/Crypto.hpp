#pragma once

// Thin wrappers over OpenSSL for the few primitives the app needs.

#include <cstddef>
#include <string>
#include <string_view>

namespace accounts::crypto {

// Lower-case hex SHA-256 of `data`.
std::string sha256Hex(std::string_view data);

// `byteCount` bytes from OpenSSL's CSPRNG, base64url-encoded without padding.
std::string randomToken(std::size_t byteCount);

std::string base64UrlEncode(std::string_view data);
// Returns an empty string for invalid input.
std::string base64UrlDecode(std::string_view encoded);

// Compares in time independent of where the inputs differ.
bool constantTimeEquals(std::string_view a, std::string_view b);

} // namespace accounts::crypto
