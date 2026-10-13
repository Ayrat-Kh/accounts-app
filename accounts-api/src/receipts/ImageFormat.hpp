#pragma once

#include <optional>
#include <string_view>

namespace accounts::receipts {

// Identifies a photo by its first bytes ("magic numbers") instead of trusting
// the client's Content-Type, so only real JPEG/PNG/WebP files are stored.
inline std::optional<std::string_view> detectImageMimeType(std::string_view bytes) {
    if (bytes.starts_with("\xFF\xD8\xFF")) {
        return "image/jpeg";
    }
    if (bytes.starts_with("\x89PNG\r\n\x1A\n")) {
        return "image/png";
    }
    if (bytes.size() >= 12 && bytes.starts_with("RIFF") && bytes.substr(8, 4) == "WEBP") {
        return "image/webp";
    }
    return std::nullopt;
}

} // namespace accounts::receipts
