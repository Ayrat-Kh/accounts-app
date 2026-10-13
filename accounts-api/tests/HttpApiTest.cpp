// End-to-end: the real uWebSockets server, router, services and Postgres,
// driven over a TCP socket. Only Google (FakeGoogle) and Garage
// (InMemoryObjectStore) are replaced.

#include "auth/AccessTokenService.hpp"
#include "auth/AuthRoutes.hpp"
#include "auth/AuthService.hpp"
#include "health/HealthRoutes.hpp"
#include "http/Server.hpp"
#include "receipts/ReceiptRoutes.hpp"
#include "storage/InMemoryObjectStore.hpp"
#include "support/TestDatabase.hpp"
#include "support/TestHttpClient.hpp"
#include "support/TestSupport.hpp"
#include "util/Crypto.hpp"

#include <boost/asio/co_spawn.hpp>
#include <boost/json/serialize.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace accounts;
using namespace std::chrono_literals;
using boost::asio::awaitable;
using testing::sendRequest;
using testing::TestRequest;
using testing::TestResponse;

namespace {

constexpr std::size_t maxImageBytes = 1024;  // tiny, so the 413 path is cheap to test

// Accepts id tokens of the form "<subject>:<email>".
class FakeGoogle : public auth::IdentityVerifier {
public:
    awaitable<Result<auth::VerifiedIdentity>> verify(std::string idToken) override {
        const auto colon = idToken.find(':');
        if (colon == std::string::npos) {
            co_return fail(ErrorCode::Unauthorized, "bad fake token");
        }
        co_return auth::VerifiedIdentity{idToken.substr(0, colon), idToken.substr(colon + 1)};
    }
};

// Everything the server needs, wired like main.cpp does.
struct TestApp {
    db::PgPool &pool = testing::testDatabase();
    storage::InMemoryObjectStore objectStore;
    auth::AccessTokenService accessTokens{testing::generateEd25519PrivateKeyPem(), "accounts-api", 900s};
    FakeGoogle google;
    auth::UserRepository users{pool};
    auth::RefreshTokenRepository refreshTokens{pool};
    auth::AuthService authService{
        google, users, refreshTokens, accessTokens,
        {.allowedEmails = {"alice@example.com", "bob@example.com"}, .refreshTokenTtl = std::chrono::days(30),
         .refreshTokenAbsoluteTtl = std::chrono::days(90)}};
    receipts::ReceiptRepository receiptRepository{pool};
    receipts::ReceiptService receiptService{receiptRepository, objectStore};
    http::Router router{accessTokens, 16 * 1024};
    http::Server server{testing::testIoContext(), router};

    TestApp() {
        health::registerHealthRoutes(router, pool);
        auth::registerAuthRoutes(router, authService);
        receipts::registerReceiptRoutes(router, receiptService, maxImageBytes);
        server.listen("127.0.0.1", 0);
    }
};

// A tiny facade so the scenario below reads like a story.
class Client {
public:
    explicit Client(std::uint16_t port) : port_(port) {}

    void signInAs(const std::string &accessToken) { accessToken_ = accessToken; }

    awaitable<TestResponse> get(std::string target) { return send({"GET", std::move(target), {}, {}}); }
    awaitable<TestResponse> remove(std::string target) { return send({"DELETE", std::move(target), {}, {}}); }
    awaitable<TestResponse> post(std::string target, const boost::json::value &body) {
        return send({"POST", std::move(target), boost::json::serialize(body), {{"Content-Type", "application/json"}}});
    }
    awaitable<TestResponse> patch(std::string target, const boost::json::value &body) {
        return send({"PATCH", std::move(target), boost::json::serialize(body), {{"Content-Type", "application/json"}}});
    }
    awaitable<TestResponse> putImage(std::string target, std::string bytes, std::string sha256) {
        return send({"PUT", std::move(target), std::move(bytes), {{"X-Content-SHA256", std::move(sha256)}}});
    }

private:
    awaitable<TestResponse> send(TestRequest request) {
        if (!accessToken_.empty()) {
            request.headers["Authorization"] = "Bearer " + accessToken_;
        }
        return sendRequest(port_, std::move(request));
    }

    std::uint16_t port_;
    std::string accessToken_;
};

std::string asString(const boost::json::value &value) {
    return std::string(value.as_string());
}

const std::string fakeJpeg = std::string("\xFF\xD8\xFF\xE0", 4) + "pretend this is a photo of a receipt";

awaitable<void> scenario(http::Server &server) {
    const std::string run = testing::uniqueSuffix();
    Client alice(server.port());
    Client bob(server.port());

    // Note: never `co_await` inside CHECK/REQUIRE. Catch2's macros repeat
    // their expression in a branch that never runs, and GCC still evaluates
    // a co_await there, which would send every request twice.

    // Health and authentication gates
    const int health = (co_await alice.get("/healthz")).status;
    const int unknownEndpoint = (co_await alice.get("/no/such/endpoint")).status;
    const int withoutToken = (co_await alice.post("/v1/receipts", {{"amount_minor", 1}})).status;
    const int notAllowlisted = (co_await alice.post("/auth/google", {{"id_token", "x:mallory@example.com"}})).status;
    CHECK(health == 200);
    CHECK(unknownEndpoint == 404);
    CHECK(withoutToken == 401);
    CHECK(notAllowlisted == 403);

    const TestResponse aliceLogin =
        co_await alice.post("/auth/google", {{"id_token", "alice-" + run + ":alice@example.com"}});
    REQUIRE(aliceLogin.status == 200);
    alice.signInAs(asString(aliceLogin.json().at("access_token")));
    const TestResponse bobLogin = co_await bob.post("/auth/google", {{"id_token", "bob-" + run + ":bob@example.com"}});
    bob.signInAs(asString(bobLogin.json().at("access_token")));

    // Create and validate
    const int lowerCaseCurrency = (co_await alice.post("/v1/receipts", {{"amount_minor", 100},
                                                                         {"currency", "eur"},
                                                                         {"purchased_at", "2026-10-01T10:00:00Z"}}))
                                      .status;
    CHECK(lowerCaseCurrency == 400);
    const TestResponse created = co_await alice.post(
        "/v1/receipts", {{"amount_minor", 1234}, {"currency", "EUR"}, {"purchased_at", "2026-10-01T10:00:00Z"},
                         {"merchant", "Lidl"}});
    REQUIRE(created.status == 201);
    const std::string receiptUrl = "/v1/receipts/" + asString(created.json().at("id"));

    // Bob cannot see or touch Alice's receipt
    const int bobReads = (co_await bob.get(receiptUrl)).status;
    const int bobEdits = (co_await bob.patch(receiptUrl, {{"amount_minor", 1}})).status;
    const int bobDeletes = (co_await bob.remove(receiptUrl)).status;
    const int bobUploads = (co_await bob.putImage(receiptUrl + "/image", fakeJpeg, crypto::sha256Hex(fakeJpeg))).status;
    CHECK(bobReads == 404);
    CHECK(bobEdits == 404);
    CHECK(bobDeletes == 404);
    CHECK(bobUploads == 404);

    // Alice edits it
    const TestResponse patched = co_await alice.patch(receiptUrl, {{"amount_minor", 1500}});
    CHECK(patched.status == 200);
    CHECK(patched.json().at("amount_minor").as_int64() == 1500);

    // Image upload: checksum, format and size are enforced
    const std::string tooLarge(maxImageBytes + 1, 'x');
    const int wrongChecksum = (co_await alice.putImage(receiptUrl + "/image", fakeJpeg, std::string(64, '0'))).status;
    const int notAnImage =
        (co_await alice.putImage(receiptUrl + "/image", "plain text", crypto::sha256Hex("plain text"))).status;
    const int oversized = (co_await alice.putImage(receiptUrl + "/image", tooLarge, crypto::sha256Hex(tooLarge))).status;
    CHECK(wrongChecksum == 400);
    CHECK(notAnImage == 400);
    CHECK(oversized == 413);

    const TestResponse uploaded = co_await alice.putImage(receiptUrl + "/image", fakeJpeg, crypto::sha256Hex(fakeJpeg));
    CHECK(uploaded.status == 200);
    CHECK(asString(uploaded.json().at("image").at("mime_type")) == "image/jpeg");
    const TestResponse downloaded = co_await alice.get(receiptUrl + "/image");
    CHECK(downloaded.status == 200);
    CHECK(downloaded.body == fakeJpeg);
    CHECK(downloaded.headers.at("content-type") == "image/jpeg");

    // Pagination: two receipts, one per page
    co_await alice.post("/v1/receipts",
                        {{"amount_minor", 1}, {"currency", "EUR"}, {"purchased_at", "2026-09-01T10:00:00Z"}});
    const TestResponse firstPage = co_await alice.get("/v1/receipts?limit=1");
    CHECK(firstPage.json().at("items").as_array().size() == 1);
    const std::string cursor = asString(firstPage.json().at("next_cursor"));
    const TestResponse secondPage = co_await alice.get("/v1/receipts?limit=1&cursor=" + cursor);
    CHECK(secondPage.json().at("items").as_array().size() == 1);
    CHECK(secondPage.json().at("next_cursor").is_null());

    // Refresh tokens rotate and are single-use
    const std::string refreshToken = asString(aliceLogin.json().at("refresh_token"));
    const int firstRefresh = (co_await alice.post("/auth/token", {{"refresh_token", refreshToken}})).status;
    const int reusedRefresh = (co_await alice.post("/auth/token", {{"refresh_token", refreshToken}})).status;
    CHECK(firstRefresh == 200);
    CHECK(reusedRefresh == 401);

    // Delete
    const int deleted = (co_await alice.remove(receiptUrl)).status;
    const int readAfterDelete = (co_await alice.get(receiptUrl)).status;
    CHECK(deleted == 204);
    CHECK(readAfterDelete == 404);

    server.stop();
}

} // namespace

TEST_CASE("the HTTP API end to end") {
    TestApp app;

    std::exception_ptr failure;
    boost::asio::co_spawn(testing::testIoContext(), scenario(app.server),
                          [&](std::exception_ptr error) {
                              failure = error;
                              app.server.stop();  // also on failure, so run() returns
                          });
    app.server.run();  // returns once the scenario stopped the server and connections closed
    if (failure) {
        std::rethrow_exception(failure);
    }
}
