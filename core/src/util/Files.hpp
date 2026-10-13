#pragma once

#include "receipts/Result.hpp"

#include <cstdint>
#include <filesystem>
#include <string>

namespace receipts::files {

Result<std::string> readAll(const std::filesystem::path &path);

// Appends to (or creates) a file.
Result<void> append(const std::filesystem::path &path, std::string_view bytes);

// Writes to "<path>.tmp" and renames it over `path`, so readers never see a
// half-written file even if the app is killed mid-write.
Result<void> writeAtomically(const std::filesystem::path &path, std::string_view bytes);

// Lower-case hex SHA-256 of a file's content, streamed (works for 300 MB models).
Result<std::string> sha256OfFile(const std::filesystem::path &path);

std::string sha256Hex(std::string_view bytes);

} // namespace receipts::files
