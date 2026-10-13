#include "util/Files.hpp"

#include <openssl/evp.h>

#include <array>
#include <fstream>
#include <memory>
#include <sstream>

namespace receipts::files {

namespace {

class Sha256 {
public:
    Sha256() : context_(EVP_MD_CTX_new(), &EVP_MD_CTX_free) { EVP_DigestInit_ex(context_.get(), EVP_sha256(), nullptr); }

    void update(const char *data, std::size_t size) { EVP_DigestUpdate(context_.get(), data, size); }

    std::string hexDigest() {
        std::array<unsigned char, EVP_MAX_MD_SIZE> digest{};
        unsigned int length = 0;
        EVP_DigestFinal_ex(context_.get(), digest.data(), &length);
        static constexpr char hexDigits[] = "0123456789abcdef";
        std::string hex;
        for (unsigned int i = 0; i < length; ++i) {
            hex.push_back(hexDigits[digest[i] >> 4]);
            hex.push_back(hexDigits[digest[i] & 0x0F]);
        }
        return hex;
    }

private:
    std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> context_;
};

} // namespace

Result<std::string> readAll(const std::filesystem::path &path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        return fail(ErrorCode::InvalidInput, "cannot read " + path.string());
    }
    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

Result<void> append(const std::filesystem::path &path, std::string_view bytes) {
    std::ofstream file(path, std::ios::binary | std::ios::app);
    file.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
    if (!file) {
        return fail(ErrorCode::Internal, "cannot write " + path.string());
    }
    return {};
}

Result<void> writeAtomically(const std::filesystem::path &path, std::string_view bytes) {
    const std::filesystem::path temporary = path.string() + ".tmp";
    {
        std::ofstream file(temporary, std::ios::binary | std::ios::trunc);
        file.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
        if (!file) {
            return fail(ErrorCode::Internal, "cannot write " + temporary.string());
        }
    }
    std::error_code error;
    std::filesystem::rename(temporary, path, error);
    if (error) {
        return fail(ErrorCode::Internal, "cannot rename to " + path.string() + ": " + error.message());
    }
    return {};
}

Result<std::string> sha256OfFile(const std::filesystem::path &path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        return fail(ErrorCode::InvalidInput, "cannot read " + path.string());
    }
    Sha256 hash;
    std::array<char, 1 << 16> chunk{};
    while (file.read(chunk.data(), chunk.size()) || file.gcount() > 0) {
        hash.update(chunk.data(), static_cast<std::size_t>(file.gcount()));
    }
    return hash.hexDigest();
}

std::string sha256Hex(std::string_view bytes) {
    Sha256 hash;
    hash.update(bytes.data(), bytes.size());
    return hash.hexDigest();
}

} // namespace receipts::files
