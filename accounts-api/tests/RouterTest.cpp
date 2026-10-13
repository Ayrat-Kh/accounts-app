#include "assets/AssetRoutes.hpp"
#include "http/Router.hpp"
#include "support/TestSupport.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace accounts;
using namespace accounts::http;
using boost::asio::awaitable;

namespace {

// Accepts exactly one token: "valid".
class FakeAuthenticator : public Authenticator {
public:
    std::optional<AuthUser> authenticate(std::string_view header) const override {
        if (header == "Bearer valid") {
            return AuthUser{"user-1"};
        }
        return std::nullopt;
    }
};

awaitable<Response> echoId(Request &request) {
    co_return jsonResponse(Status::Ok, {{"id", request.pathParam("id")}, {"user", request.authenticatedUser().userId}});
}

} // namespace

TEST_CASE("routes match by method and path, capturing :params") {
    FakeAuthenticator authenticator;
    Router router(authenticator, 1024);
    router.add(Method::Get, "/v1/receipts/:id", echoId);

    const auto match = router.match(Method::Get, "/v1/receipts/abc");
    REQUIRE(match);
    CHECK(match->pathParams.at("id") == "abc");

    CHECK_FALSE(router.match(Method::Post, "/v1/receipts/abc"));
    CHECK_FALSE(router.match(Method::Get, "/v1/receipts"));
    CHECK_FALSE(router.match(Method::Get, "/v1/receipts/abc/extra"));
}

TEST_CASE("authenticated routes reject requests without a valid token") {
    FakeAuthenticator authenticator;
    Router router(authenticator, 1024);
    router.add(Method::Get, "/v1/receipts/:id", echoId);
    const auto match = router.match(Method::Get, "/v1/receipts/abc").value();

    Request anonymous;
    CHECK(testing::runSync(router.dispatch(match, anonymous)).status == Status::Unauthorized);

    Request signedIn;
    signedIn.headers["authorization"] = "Bearer valid";
    const Response response = testing::runSync(router.dispatch(match, signedIn));
    CHECK(response.status == Status::Ok);
    CHECK(response.body == R"({"id":"abc","user":"user-1"})");
}

TEST_CASE("each route has its own body limit, defaulting to the router's") {
    FakeAuthenticator authenticator;
    Router router(authenticator, 1024);
    router.add(Method::Post, "/small", echoId);
    router.add(Method::Put, "/large", echoId, {.requiresAuth = true, .maxBodyBytes = 10'000'000});
    CHECK(router.maxBodyBytes(router.match(Method::Post, "/small").value()) == 1024);
    CHECK(router.maxBodyBytes(router.match(Method::Put, "/large").value()) == 10'000'000);
}

TEST_CASE("an exception escaping a handler becomes a 500, not a crash") {
    FakeAuthenticator authenticator;
    Router router(authenticator, 1024);
    router.add(
        Method::Get, "/boom", [](Request &) -> awaitable<Response> { throw std::runtime_error("bug"); },
        {.requiresAuth = false, .maxBodyBytes = {}});
    const Response response = testing::runSync(router.dispatch(router.match(Method::Get, "/boom").value(), {}));
    CHECK(response.status == Status::InternalServerError);
}

TEST_CASE("query strings are split and percent-decoded") {
    const auto params = parseQueryString("cursor=a%2Bb&limit=5&empty=&flag");
    CHECK(params.at("cursor") == "a+b");
    CHECK(params.at("limit") == "5");
    CHECK(params.at("empty").empty());
    CHECK(params.contains("flag"));
}

TEST_CASE("HTTP methods parse case-insensitively") {
    CHECK(parseMethod("get") == Method::Get);
    CHECK(parseMethod("PATCH") == Method::Patch);
    CHECK(parseMethod("BREW") == Method::Other);
}

TEST_CASE("Range headers parse as inclusive byte ranges") {
    const auto range = assets::parseRangeHeader("bytes=100-199");
    REQUIRE(range);
    CHECK(range->first == 100);
    CHECK(range->last == 199);
    CHECK_FALSE(assets::parseRangeHeader("bytes=200-100"));
    CHECK_FALSE(assets::parseRangeHeader("bytes=100-"));
    CHECK_FALSE(assets::parseRangeHeader("items=0-1"));
}
