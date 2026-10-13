#include "support/TestSupport.hpp"

#include "util/Crypto.hpp"

#include <openssl/evp.h>
#include <openssl/pem.h>

#include <memory>
#include <stdexcept>

namespace accounts::testing {

boost::asio::io_context &testIoContext() {
    static boost::asio::io_context ioContext;
    return ioContext;
}

std::string generateEd25519PrivateKeyPem() {
    std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> key(EVP_PKEY_Q_keygen(nullptr, nullptr, "ED25519"),
                                                            &EVP_PKEY_free);
    std::unique_ptr<BIO, decltype(&BIO_free)> bio(BIO_new(BIO_s_mem()), &BIO_free);
    if (!key || !bio || PEM_write_bio_PrivateKey(bio.get(), key.get(), nullptr, nullptr, 0, nullptr, nullptr) != 1) {
        throw std::runtime_error("cannot generate an Ed25519 key");
    }
    char *data = nullptr;
    const long length = BIO_get_mem_data(bio.get(), &data);
    return {data, static_cast<std::size_t>(length)};
}

std::string uniqueSuffix() {
    return crypto::randomToken(9);
}

} // namespace accounts::testing
