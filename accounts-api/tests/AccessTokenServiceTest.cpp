#include "auth/AccessTokenService.hpp"
#include "support/TestSupport.hpp"

#include <catch2/catch_test_macros.hpp>

using accounts::auth::AccessTokenService;
using accounts::testing::generateEd25519PrivateKeyPem;
using namespace std::chrono_literals;

namespace {

AccessTokenService makeService(std::string key = generateEd25519PrivateKeyPem(), std::string issuer = "accounts-api",
                               std::chrono::seconds ttl = 900s) {
    return AccessTokenService(std::move(key), std::move(issuer), ttl);
}

} // namespace

TEST_CASE("an issued access token verifies and names its user") {
    const auto service = makeService();
    const auto user = service.verify(service.issue("user-1"));
    REQUIRE(user);
    CHECK(user->userId == "user-1");
}

TEST_CASE("authenticate reads the token from a Bearer header") {
    const auto service = makeService();
    CHECK(service.authenticate("Bearer " + service.issue("user-1"))->userId == "user-1");
    CHECK_FALSE(service.authenticate(""));
}

TEST_CASE("a token signed with a different key is rejected") {
    const auto ours = makeService();
    const auto theirs = makeService();
    CHECK_FALSE(ours.verify(theirs.issue("user-1")));
}

TEST_CASE("a token from a different issuer is rejected") {
    const std::string key = generateEd25519PrivateKeyPem();
    const auto ours = makeService(key, "accounts-api");
    const auto other = makeService(key, "someone-else");
    CHECK_FALSE(ours.verify(other.issue("user-1")));
}

TEST_CASE("an expired token is rejected") {
    // Expired a minute ago: beyond the 30 s clock-skew leeway.
    const auto service = makeService(generateEd25519PrivateKeyPem(), "accounts-api", -60s);
    CHECK_FALSE(service.verify(service.issue("user-1")));
}

TEST_CASE("a tampered token is rejected") {
    const auto service = makeService();
    std::string token = service.issue("user-1");
    token[token.size() / 2] = token[token.size() / 2] == 'A' ? 'B' : 'A';
    CHECK_FALSE(service.verify(token));
    CHECK_FALSE(service.verify("not-a-jwt"));
}
