#pragma once

// Sign-in, token refresh and sign-out: the business rules of authentication.
//
//   signInWithGoogle(idToken)  Google says who it is -> allowlist check ->
//                              user row -> fresh token pair
//   refresh(refreshToken)      exchange a refresh token for a new pair; the
//                              old one is revoked (rotation), so a leaked
//                              token stops working as soon as either party uses it
//   signOut(refreshToken)      revoke it

#include "auth/AccessTokenService.hpp"
#include "auth/IdentityVerifier.hpp"
#include "auth/RefreshTokenRepository.hpp"
#include "auth/UserRepository.hpp"

#include <chrono>
#include <string>
#include <vector>

namespace accounts::auth {

struct TokenPair {
    std::string accessToken;
    std::chrono::seconds accessTokenExpiresIn;
    std::string refreshToken;
};

struct AuthPolicy {
    std::vector<std::string> allowedEmails;
    std::chrono::days refreshTokenTtl;
    std::chrono::days refreshTokenAbsoluteTtl;
};

class AuthService {
public:
    AuthService(IdentityVerifier &identityVerifier, UserRepository &users, RefreshTokenRepository &refreshTokens,
                const AccessTokenService &accessTokens, AuthPolicy policy);

    boost::asio::awaitable<Result<TokenPair>> signInWithGoogle(std::string idToken);
    boost::asio::awaitable<Result<TokenPair>> refresh(std::string refreshToken);
    boost::asio::awaitable<Result<void>> signOut(std::string refreshToken);

private:
    bool isAllowed(const std::string &email) const;
    boost::asio::awaitable<Result<TokenPair>> issueTokens(std::string userId, Timestamp absoluteExpiresAt);

    IdentityVerifier &identityVerifier_;
    UserRepository &users_;
    RefreshTokenRepository &refreshTokens_;
    const AccessTokenService &accessTokens_;
    AuthPolicy policy_;
};

} // namespace accounts::auth
