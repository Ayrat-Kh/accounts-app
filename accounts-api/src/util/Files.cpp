#include "util/Files.hpp"

#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace accounts {

std::string readFile(const std::string &path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("cannot open file: " + path);
    }
    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

std::string readSecretFile(const std::string &path) {
    std::string secret = readFile(path);
    while (!secret.empty() && std::isspace(static_cast<unsigned char>(secret.back()))) {
        secret.pop_back();
    }
    return secret;
}

} // namespace accounts
