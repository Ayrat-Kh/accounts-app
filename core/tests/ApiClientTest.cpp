#include "api/ApiClient.hpp"
#include "support/Fakes.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace receipts;
using namespace receipts::testing;

namespace {

const std::string tokens = R"({"access_token": "access-2", "token_type": "Bearer", "expires_in": 900,
                                "refresh_token": "refresh-2"})";
const std::string receiptJson = R"({"id": "r-1", "amount_minor": 1234, "currency": "EUR",
    "purchased_at": "2026-10-13T12:00:00.000Z", "merchant": "Lidl", "ocr_confidence": 0.9, "image": null,
    "created_at": "2026-10-13T12:00:00.000Z", "updated_at": "2026-10-13T12:00:00.000Z"})";

std::shared_ptr<MemorySecureStore> signedInStore() {
    auto store = std::make_shared<MemorySecureStore>();
    store->write("accounts.access_token", "access-1");
    store->write("accounts.refresh_token", "refresh-1");
    return store;
}

} // namespace

TEST_CASE("signing in stores both tokens") {
    auto transport =
        std::make_shared<FakeHttpTransport>([](const net::HttpRequest &) { return jsonResponse(200, tokens); });
    auto secrets = std::make_shared<MemorySecureStore>();
    api::ApiClient api(transport, "https://api.test", secrets);

    REQUIRE(api.signInWithGoogle("google-id-token"));
    CHECK(api.isSignedIn());
    CHECK(secrets->values.at("accounts.access_token") == "access-2");
    CHECK(transport->requests[0].body.find("google-id-token") != std::string::npos);
}

TEST_CASE("requests carry the access token") {
    auto transport =
        std::make_shared<FakeHttpTransport>([](const net::HttpRequest &) { return jsonResponse(201, receiptJson); });
    api::ApiClient api(transport, "https://api.test", signedInStore());

    const auto created = api.createReceipt({.money = {1234, "EUR"}, .purchasedAt = "2026-10-13T12:00:00.000Z"});
    REQUIRE(created);
    CHECK(created->serverId == "r-1");
    CHECK(transport->requests[0].headers.at("Authorization") == "Bearer access-1");
}

TEST_CASE("an expired access token is refreshed once and the request retried") {
    auto transport = std::make_shared<FakeHttpTransport>([](const net::HttpRequest &request) {
        if (request.url.ends_with("/auth/token")) {
            return jsonResponse(200, tokens);
        }
        const bool fresh = request.headers.at("Authorization") == "Bearer access-2";
        return fresh ? jsonResponse(200, R"({"items": [], "next_cursor": null})")
                     : jsonResponse(401, R"({"error": {"code": "unauthorized", "message": "expired"}})");
    });
    auto secrets = signedInStore();
    api::ApiClient api(transport, "https://api.test", secrets);

    REQUIRE(api.listAllReceipts());
    CHECK(transport->requests.size() == 3); // 401, refresh, retry
    CHECK(secrets->values.at("accounts.refresh_token") == "refresh-2");
}

TEST_CASE("a rejected refresh token signs the user out") {
    auto transport = std::make_shared<FakeHttpTransport>(
        [](const net::HttpRequest &) { return jsonResponse(401, R"({"error": {"message": "nope"}})"); });
    api::ApiClient api(transport, "https://api.test", signedInStore());

    const auto result = api.listAllReceipts();
    REQUIRE_FALSE(result);
    CHECK(result.error().code == ErrorCode::NotSignedIn);
    CHECK_FALSE(api.isSignedIn());
}

TEST_CASE("listing follows the pagination cursor to the end") {
    auto transport = std::make_shared<FakeHttpTransport>([](const net::HttpRequest &request) {
        const bool secondPage = request.url.find("cursor=page2") != std::string::npos;
        return jsonResponse(200, "{\"items\": [" + receiptJson +
                                     "], \"next_cursor\": " + (secondPage ? "null" : "\"page2\"") + "}");
    });
    api::ApiClient api(transport, "https://api.test", signedInStore());

    CHECK(api.listAllReceipts().value().size() == 2);
}

TEST_CASE("server errors keep the server's message") {
    auto transport = std::make_shared<FakeHttpTransport>([](const net::HttpRequest &) {
        return jsonResponse(400, R"({"error": {"code": "invalid_input", "message": "currency must be ISO"}})");
    });
    api::ApiClient api(transport, "https://api.test", signedInStore());

    const auto result = api.createReceipt({.money = {1, "eur"}, .purchasedAt = "2026-10-13T12:00:00.000Z"});
    REQUIRE_FALSE(result);
    CHECK(result.error().code == ErrorCode::InvalidInput);
    CHECK(result.error().message == "currency must be ISO");
}

TEST_CASE("image uploads carry the SHA-256 the server checks") {
    auto transport =
        std::make_shared<FakeHttpTransport>([](const net::HttpRequest &) { return jsonResponse(200, receiptJson); });
    api::ApiClient api(transport, "https://api.test", signedInStore());

    REQUIRE(api.uploadImage("r-1", "abc"));
    CHECK(transport->requests[0].headers.at("X-Content-SHA256") ==
          "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}
