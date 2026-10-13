#include "auth/BearerToken.hpp"

#include <catch2/catch_test_macros.hpp>

using accounts::auth::parseBearerToken;

TEST_CASE("parseBearerToken extracts the token after the Bearer scheme") {
    CHECK(parseBearerToken("Bearer abc.def.ghi") == "abc.def.ghi");
}

TEST_CASE("parseBearerToken treats the scheme case-insensitively") {
    CHECK(parseBearerToken("bearer token") == "token");
    CHECK(parseBearerToken("BEARER token") == "token");
}

TEST_CASE("parseBearerToken rejects headers that are too short to hold a token") {
    // The old code called remove_prefix(7) on these: undefined behaviour.
    CHECK_FALSE(parseBearerToken(""));
    CHECK_FALSE(parseBearerToken("Bear"));
    CHECK_FALSE(parseBearerToken("Bearer "));
    CHECK_FALSE(parseBearerToken("Bearer    "));
}

TEST_CASE("parseBearerToken rejects other schemes and malformed tokens") {
    CHECK_FALSE(parseBearerToken("Basic dXNlcjpwYXNz"));
    CHECK_FALSE(parseBearerToken("Bearer two tokens"));
}
