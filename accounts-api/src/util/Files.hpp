#pragma once

#include <string>

namespace accounts {

// Reads a whole file. Throws std::runtime_error if it cannot be opened: files
// are only read at startup, where failing loudly is what we want.
std::string readFile(const std::string &path);

// Like readFile, but strips trailing whitespace. Secrets are mounted from
// Kubernetes Secrets, and a stray "\n" would silently become part of a password.
std::string readSecretFile(const std::string &path);

} // namespace accounts
