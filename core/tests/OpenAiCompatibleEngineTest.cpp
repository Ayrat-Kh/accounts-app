#include "inference/OpenAiCompatibleEngine.hpp"
#include "support/Fakes.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <nlohmann/json.hpp>

using namespace receipts;
using namespace receipts::testing;

namespace {

EngineConfig serverConfig() {
    return {.kind = EngineConfig::Kind::OpenAiCompatible, .baseUrl = "http://localhost:8080/", .model = "gemma"};
}

} // namespace

TEST_CASE("embeddings come back normalised and in input order") {
    auto transport = std::make_shared<FakeHttpTransport>([](const net::HttpRequest &) {
        // Deliberately out of order and not normalised.
        return jsonResponse(200, R"({"data": [{"index": 1, "embedding": [0, 2]}, {"index": 0, "embedding": [3, 4]}]})");
    });
    inference::OpenAiCompatibleEngine engine(transport, serverConfig(), std::nullopt);

    const auto embeddings = engine.embed({"first", "second"}).value();
    CHECK(embeddings[0][0] == Catch::Approx(0.6));
    CHECK(embeddings[0][1] == Catch::Approx(0.8));
    CHECK(embeddings[1][1] == Catch::Approx(1.0));

    const auto &request = transport->requests.at(0);
    CHECK(request.url == "http://localhost:8080/v1/embeddings");
    CHECK(nlohmann::json::parse(request.body)["input"][1] == "second");
}

TEST_CASE("completions ask for structured JSON output and return the message content") {
    auto transport = std::make_shared<FakeHttpTransport>([](const net::HttpRequest &) {
        return jsonResponse(200, R"({"choices": [{"message": {"content": "{\"total\": \"3.78\"}"}}]})");
    });
    inference::OpenAiCompatibleEngine engine(transport, serverConfig(), std::string("secret"));

    CHECK(engine.completeJson("instructions", "receipt text", R"({"type": "object"})").value() ==
          R"({"total": "3.78"})");
    const auto body = nlohmann::json::parse(transport->requests.at(0).body);
    CHECK(body["response_format"]["type"] == "json_schema");
    CHECK(body["messages"][1]["content"] == "receipt text");
    CHECK(transport->requests.at(0).headers.at("Authorization") == "Bearer secret");
}

TEST_CASE("server failures become errors, not crashes") {
    auto transport =
        std::make_shared<FakeHttpTransport>([](const net::HttpRequest &) { return jsonResponse(503, "busy"); });
    inference::OpenAiCompatibleEngine engine(transport, serverConfig(), std::nullopt);
    CHECK(engine.embed({"x"}).error().code == ErrorCode::Server);

    auto garbled =
        std::make_shared<FakeHttpTransport>([](const net::HttpRequest &) { return jsonResponse(200, "{}"); });
    inference::OpenAiCompatibleEngine garbledEngine(garbled, serverConfig(), std::nullopt);
    CHECK(garbledEngine.embed({"x"}).error().code == ErrorCode::Server);
}
