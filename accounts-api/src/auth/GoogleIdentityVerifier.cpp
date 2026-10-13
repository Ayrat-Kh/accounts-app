#include "auth/GoogleIdentityVerifier.hpp"

#include <boost/json/parse.hpp>

#include <algorithm>
#include <cctype>
#include <charconv>
#include <chrono>

namespace accounts::auth {

using boost::asio::awaitable;

namespace {

constexpr std::string_view googleHost = "oauth2.googleapis.com";

std::string stringClaim(const boost::json::object &claims, std::string_view key) {
    const auto *value = claims.if_contains(key);
    return (value && value->is_string()) ? std::string(value->as_string()) : std::string{};
}

bool isTokenSafeForUrl(std::string_view token) {
    // JWTs are base64url segments joined by dots; anything else is not a token
    // and must not be pasted into a query string.
    return !token.empty() && token.size() < 4096 && std::ranges::all_of(token, [](char c) {
        return std::isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_' || c == '.';
    });
}

} // namespace

Result<VerifiedIdentity> checkGoogleTokenInfo(const boost::json::object &tokenInfo,
                                              const std::vector<std::string> &allowedClientIds) {
    const std::string audience = stringClaim(tokenInfo, "aud");
    if (std::ranges::find(allowedClientIds, audience) == allowedClientIds.end()) {
        return fail(ErrorCode::Unauthorized, "Google token was issued for a different app");
    }

    const std::string issuer = stringClaim(tokenInfo, "iss");
    if (issuer != "accounts.google.com" && issuer != "https://accounts.google.com") {
        return fail(ErrorCode::Unauthorized, "token was not issued by Google");
    }

    // tokeninfo returns numbers as strings, e.g. "exp": "1760000000".
    const std::string expiresAtText = stringClaim(tokenInfo, "exp");
    std::int64_t expiresAt = 0;
    std::from_chars(expiresAtText.data(), expiresAtText.data() + expiresAtText.size(), expiresAt);
    const auto nowSeconds =
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    if (expiresAt <= nowSeconds) {
        return fail(ErrorCode::Unauthorized, "Google token has expired");
    }

    if (stringClaim(tokenInfo, "email_verified") != "true") {
        return fail(ErrorCode::Unauthorized, "Google account e-mail is not verified");
    }

    VerifiedIdentity identity{stringClaim(tokenInfo, "sub"), stringClaim(tokenInfo, "email")};
    if (identity.subject.empty() || identity.email.empty()) {
        return fail(ErrorCode::Unauthorized, "Google token has no subject or e-mail");
    }
    return identity;
}

GoogleIdentityVerifier::GoogleIdentityVerifier(net::HttpsClient &httpsClient, std::vector<std::string> allowedClientIds)
    : httpsClient_(httpsClient), allowedClientIds_(std::move(allowedClientIds)) {}

awaitable<Result<VerifiedIdentity>> GoogleIdentityVerifier::verify(std::string idToken) {
    if (!isTokenSafeForUrl(idToken)) {
        co_return fail(ErrorCode::InvalidInput, "id_token is malformed");
    }
    auto response = co_await httpsClient_.get(std::string(googleHost), "/tokeninfo?id_token=" + idToken);
    if (!response) {
        co_return std::unexpected(response.error());
    }
    // Google answers 400 for invalid or expired tokens.
    if (response->status != 200) {
        co_return fail(ErrorCode::Unauthorized, "Google rejected the token");
    }

    boost::system::error_code parseError;
    const auto tokenInfo = boost::json::parse(response->body, parseError);
    if (parseError || !tokenInfo.is_object()) {
        co_return fail(ErrorCode::UpstreamFailure, "Google tokeninfo returned invalid JSON");
    }
    co_return checkGoogleTokenInfo(tokenInfo.as_object(), allowedClientIds_);
}

} // namespace accounts::auth
