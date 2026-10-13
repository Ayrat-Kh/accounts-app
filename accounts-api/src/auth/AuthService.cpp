#include "auth/AuthService.hpp"

#include "util/Crypto.hpp"

#include <algorithm>
#include <cctype>

namespace accounts::auth {

using boost::asio::awaitable;

namespace {

constexpr std::size_t refreshTokenBytes = 32;  // 256 bits of randomness

bool equalsIgnoringCase(std::string_view a, std::string_view b) {
    return std::ranges::equal(a, b, [](char x, char y) {
        return std::tolower(static_cast<unsigned char>(x)) == std::tolower(static_cast<unsigned char>(y));
    });
}

} // namespace

AuthService::AuthService(IdentityVerifier &identityVerifier, UserRepository &users,
                         RefreshTokenRepository &refreshTokens, const AccessTokenService &accessTokens,
                         AuthPolicy policy)
    : identityVerifier_(identityVerifier), users_(users), refreshTokens_(refreshTokens), accessTokens_(accessTokens),
      policy_(std::move(policy)) {}

bool AuthService::isAllowed(const std::string &email) const {
    return std::ranges::any_of(policy_.allowedEmails,
                               [&](const std::string &allowed) { return equalsIgnoringCase(allowed, email); });
}

awaitable<Result<TokenPair>> AuthService::signInWithGoogle(std::string idToken) {
    auto identity = co_await identityVerifier_.verify(std::move(idToken));
    if (!identity) {
        co_return std::unexpected(identity.error());
    }
    if (!isAllowed(identity->email)) {
        co_return fail(ErrorCode::Forbidden, "this Google account is not allowed to sign in");
    }
    auto user = co_await users_.upsertByGoogleSubject(identity->subject, identity->email);
    if (!user) {
        co_return std::unexpected(user.error());
    }
    co_return co_await issueTokens(user->id, now() + policy_.refreshTokenAbsoluteTtl);
}

awaitable<Result<TokenPair>> AuthService::refresh(std::string refreshToken) {
    auto stored = co_await refreshTokens_.consume(crypto::sha256Hex(refreshToken));
    if (!stored) {
        co_return std::unexpected(stored.error());
    }
    if (!*stored) {
        co_return fail(ErrorCode::Unauthorized, "refresh token is invalid, expired or already used");
    }
    // The new token inherits the absolute deadline of the session it continues.
    co_return co_await issueTokens((*stored)->userId, (*stored)->absoluteExpiresAt);
}

awaitable<Result<void>> AuthService::signOut(std::string refreshToken) {
    co_return co_await refreshTokens_.revoke(crypto::sha256Hex(refreshToken));
}

awaitable<Result<TokenPair>> AuthService::issueTokens(std::string userId, Timestamp absoluteExpiresAt) {
    const std::string refreshToken = crypto::randomToken(refreshTokenBytes);
    const Timestamp expiresAt = std::min(now() + policy_.refreshTokenTtl, absoluteExpiresAt);

    auto stored =
        co_await refreshTokens_.insert(userId, crypto::sha256Hex(refreshToken), expiresAt, absoluteExpiresAt);
    if (!stored) {
        co_return std::unexpected(stored.error());
    }
    co_return TokenPair{accessTokens_.issue(userId), accessTokens_.ttl(), refreshToken};
}

} // namespace accounts::auth
