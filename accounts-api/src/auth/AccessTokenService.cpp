#include "auth/AccessTokenService.hpp"

#include "auth/BearerToken.hpp"

#include <jwt-cpp/traits/boost-json/defaults.h>

namespace accounts::auth {

namespace {

constexpr std::string_view accessTokenType = "access";

// jwt-cpp derives the public key from the private one; the empty strings are
// the (unused) public key and the two key passwords.
jwt::algorithm::ed25519 signingAlgorithm(const std::string &privateKeyPem) {
    return jwt::algorithm::ed25519("", privateKeyPem, "", "");
}

} // namespace

AccessTokenService::AccessTokenService(std::string privateKeyPem, std::string issuer, std::chrono::seconds ttl)
    : privateKeyPem_(std::move(privateKeyPem)), issuer_(std::move(issuer)), ttl_(ttl) {
    // Fail at startup, not on the first login, if the key is unusable.
    (void)issue("startup-self-test");
}

std::string AccessTokenService::issue(const std::string &userId) const {
    const auto now = std::chrono::system_clock::now();
    return jwt::create()
        .set_type("JWT")
        .set_issuer(issuer_)
        .set_subject(userId)
        .set_issued_at(now)
        .set_expires_at(now + ttl_)
        .set_payload_claim("typ", jwt::claim(std::string(accessTokenType)))
        .sign(signingAlgorithm(privateKeyPem_));
}

std::optional<AuthUser> AccessTokenService::verify(std::string_view token) const {
    try {
        const auto decoded = jwt::decode(std::string(token));
        jwt::verify()
            .allow_algorithm(signingAlgorithm(privateKeyPem_))  // pins EdDSA: no "alg: none" tricks
            .with_issuer(issuer_)
            .with_claim("typ", jwt::claim(std::string(accessTokenType)))
            .leeway(30)  // seconds of clock skew
            .verify(decoded);
        if (!decoded.has_subject() || decoded.get_subject().empty()) {
            return std::nullopt;
        }
        return AuthUser{decoded.get_subject()};
    } catch (const std::exception &) {
        // Malformed, forged or expired. The token itself is never logged.
        return std::nullopt;
    }
}

std::optional<AuthUser> AccessTokenService::authenticate(std::string_view authorizationHeader) const {
    const auto token = parseBearerToken(authorizationHeader);
    return token ? verify(*token) : std::nullopt;
}

} // namespace accounts::auth
