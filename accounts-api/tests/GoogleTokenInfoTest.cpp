#include "auth/GoogleIdentityVerifier.hpp"

#include <catch2/catch_test_macros.hpp>

#include <chrono>

using accounts::ErrorCode;
using accounts::auth::checkGoogleTokenInfo;

namespace {

const std::vector<std::string> ourClientIds = {"our-android-client", "our-web-client"};

std::string secondsFromNow(int seconds) {
    const auto now = std::chrono::system_clock::now() + std::chrono::seconds(seconds);
    return std::to_string(std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count());
}

// A tokeninfo answer that passes every check; tests break one field at a time.
boost::json::object validTokenInfo() {
    return {
        {"aud", "our-android-client"}, {"iss", "https://accounts.google.com"},
        {"exp", secondsFromNow(600)},  {"email_verified", "true"},
        {"sub", "1234567890"},         {"email", "me@example.com"},
    };
}

} // namespace

TEST_CASE("a valid Google token yields the subject and e-mail") {
    const auto identity = checkGoogleTokenInfo(validTokenInfo(), ourClientIds);
    REQUIRE(identity);
    CHECK(identity->subject == "1234567890");
    CHECK(identity->email == "me@example.com");
}

TEST_CASE("a Google token issued to another app is rejected") {
    // The old backend skipped this check: any app's token could log in.
    auto tokenInfo = validTokenInfo();
    tokenInfo["aud"] = "some-other-app";
    CHECK(checkGoogleTokenInfo(tokenInfo, ourClientIds).error().code == ErrorCode::Unauthorized);
}

TEST_CASE("a token from an issuer other than Google is rejected") {
    auto tokenInfo = validTokenInfo();
    tokenInfo["iss"] = "https://evil.example.com";
    CHECK_FALSE(checkGoogleTokenInfo(tokenInfo, ourClientIds));
}

TEST_CASE("an expired Google token is rejected") {
    auto tokenInfo = validTokenInfo();
    tokenInfo["exp"] = secondsFromNow(-1);
    CHECK_FALSE(checkGoogleTokenInfo(tokenInfo, ourClientIds));
}

TEST_CASE("a Google account with an unverified e-mail is rejected") {
    auto tokenInfo = validTokenInfo();
    tokenInfo["email_verified"] = "false";
    CHECK_FALSE(checkGoogleTokenInfo(tokenInfo, ourClientIds));
}
