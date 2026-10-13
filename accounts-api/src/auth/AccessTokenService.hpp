#pragma once

// Issues and verifies our own short-lived access tokens: JWTs signed with
// Ed25519. A stolen access token is useful for at most `ttl` (15 minutes by
// default); long-lived sessions use rotating refresh tokens (AuthService).
//
// Claims: iss (our issuer), sub (user id), iat, exp, typ="access".

#include "auth/AuthUser.hpp"
#include "http/Router.hpp"

#include <chrono>
#include <optional>
#include <string>
#include <string_view>

namespace accounts::auth {

class AccessTokenService : public http::Authenticator {
public:
    AccessTokenService(std::string privateKeyPem, std::string issuer, std::chrono::seconds ttl);

    std::string issue(const std::string &userId) const;

    std::chrono::seconds ttl() const { return ttl_; }

    // nullopt for a bad signature, wrong issuer/type, or an expired token.
    std::optional<AuthUser> verify(std::string_view token) const;

    // http::Authenticator
    std::optional<AuthUser> authenticate(std::string_view authorizationHeader) const override;

private:
    std::string privateKeyPem_;
    std::string issuer_;
    std::chrono::seconds ttl_;
};

} // namespace accounts::auth
