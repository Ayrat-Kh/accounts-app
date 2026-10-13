#include "util/Crypto.hpp"

#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <array>
#include <stdexcept>
#include <vector>

namespace accounts::crypto {

std::string sha256Hex(std::string_view data) {
    std::array<unsigned char, EVP_MAX_MD_SIZE> digest{};
    unsigned int digestLength = 0;
    if (EVP_Digest(data.data(), data.size(), digest.data(), &digestLength, EVP_sha256(), nullptr) != 1) {
        throw std::runtime_error("EVP_Digest(sha256) failed");
    }
    static constexpr char hexDigits[] = "0123456789abcdef";
    std::string hex;
    hex.reserve(digestLength * 2);
    for (unsigned int i = 0; i < digestLength; ++i) {
        hex.push_back(hexDigits[digest[i] >> 4]);
        hex.push_back(hexDigits[digest[i] & 0x0F]);
    }
    return hex;
}

std::string randomToken(std::size_t byteCount) {
    std::vector<unsigned char> bytes(byteCount);
    if (RAND_bytes(bytes.data(), static_cast<int>(bytes.size())) != 1) {
        throw std::runtime_error("RAND_bytes failed");
    }
    return base64UrlEncode({reinterpret_cast<const char *>(bytes.data()), bytes.size()});
}

std::string base64UrlEncode(std::string_view data) {
    static constexpr char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
    std::string encoded;
    encoded.reserve((data.size() + 2) / 3 * 4);
    std::size_t i = 0;
    for (; i + 2 < data.size(); i += 3) {
        const auto chunk = (static_cast<unsigned char>(data[i]) << 16) |
                           (static_cast<unsigned char>(data[i + 1]) << 8) | static_cast<unsigned char>(data[i + 2]);
        encoded.push_back(alphabet[(chunk >> 18) & 0x3F]);
        encoded.push_back(alphabet[(chunk >> 12) & 0x3F]);
        encoded.push_back(alphabet[(chunk >> 6) & 0x3F]);
        encoded.push_back(alphabet[chunk & 0x3F]);
    }
    const std::size_t remaining = data.size() - i;
    if (remaining == 1) {
        const auto chunk = static_cast<unsigned char>(data[i]) << 16;
        encoded.push_back(alphabet[(chunk >> 18) & 0x3F]);
        encoded.push_back(alphabet[(chunk >> 12) & 0x3F]);
    } else if (remaining == 2) {
        const auto chunk = (static_cast<unsigned char>(data[i]) << 16) | (static_cast<unsigned char>(data[i + 1]) << 8);
        encoded.push_back(alphabet[(chunk >> 18) & 0x3F]);
        encoded.push_back(alphabet[(chunk >> 12) & 0x3F]);
        encoded.push_back(alphabet[(chunk >> 6) & 0x3F]);
    }
    return encoded;
}

std::string base64UrlDecode(std::string_view encoded) {
    auto valueOf = [](char c) -> int {
        if (c >= 'A' && c <= 'Z') return c - 'A';
        if (c >= 'a' && c <= 'z') return c - 'a' + 26;
        if (c >= '0' && c <= '9') return c - '0' + 52;
        if (c == '-') return 62;
        if (c == '_') return 63;
        return -1;
    };
    if (encoded.size() % 4 == 1) {
        return {};
    }
    std::string decoded;
    unsigned int buffer = 0;
    int bitCount = 0;
    for (char c : encoded) {
        const int value = valueOf(c);
        if (value < 0) {
            return {};
        }
        buffer = (buffer << 6) | static_cast<unsigned int>(value);
        bitCount += 6;
        if (bitCount >= 8) {
            bitCount -= 8;
            decoded.push_back(static_cast<char>((buffer >> bitCount) & 0xFF));
        }
    }
    return decoded;
}

bool constantTimeEquals(std::string_view a, std::string_view b) {
    return a.size() == b.size() && CRYPTO_memcmp(a.data(), b.data(), a.size()) == 0;
}

} // namespace accounts::crypto
