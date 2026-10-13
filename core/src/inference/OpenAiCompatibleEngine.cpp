#include "inference/OpenAiCompatibleEngine.hpp"

#include "inference/VectorMath.hpp"

#include <nlohmann/json.hpp>

namespace receipts::inference {

using nlohmann::json;

namespace {

std::string withoutTrailingSlash(std::string url) {
    while (!url.empty() && url.back() == '/') {
        url.pop_back();
    }
    return url;
}

} // namespace

OpenAiCompatibleEngine::OpenAiCompatibleEngine(std::shared_ptr<net::HttpTransport> transport, EngineConfig config,
                                               std::optional<std::string> apiKey)
    : transport_(std::move(transport)), config_(std::move(config)), apiKey_(std::move(apiKey)) {
    config_.baseUrl = withoutTrailingSlash(config_.baseUrl);
}

std::string OpenAiCompatibleEngine::modelId() const {
    return config_.baseUrl + "#" + config_.model;
}

Result<std::string> OpenAiCompatibleEngine::post(const std::string &path, const std::string &body) {
    net::HttpRequest request{
        .method = "POST",
        .url = config_.baseUrl + path,
        .headers = {{"Content-Type", "application/json"}},
        .body = body,
        .timeout = config_.timeout,
    };
    if (apiKey_) {
        request.headers["Authorization"] = "Bearer " + *apiKey_;
    }
    auto response = transport_->send(request);
    if (!response) {
        return std::unexpected(response.error());
    }
    if (!response->isSuccess()) {
        return fail(ErrorCode::Server, "model server answered " + std::to_string(response->status) + " for " + path);
    }
    return std::move(response->body);
}

Result<std::vector<Embedding>> OpenAiCompatibleEngine::embed(const std::vector<std::string> &texts) {
    auto body = post("/v1/embeddings", json{{"model", config_.model}, {"input", texts}}.dump());
    if (!body) {
        return std::unexpected(body.error());
    }
    try {
        // {"data": [{"index": 0, "embedding": [...]}, ...]}; index restores the input order.
        const json parsed = json::parse(*body);
        std::vector<Embedding> embeddings(texts.size());
        for (const json &item : parsed.at("data")) {
            const auto index = item.at("index").get<std::size_t>();
            if (index >= embeddings.size()) {
                return fail(ErrorCode::Server, "embedding index out of range");
            }
            embeddings[index] = item.at("embedding").get<Embedding>();
            normalize(embeddings[index]); // servers differ on whether they normalise
        }
        return embeddings;
    } catch (const json::exception &error) {
        return fail(ErrorCode::Server, std::string("unexpected /v1/embeddings response: ") + error.what());
    }
}

Result<std::string> OpenAiCompatibleEngine::completeJson(const std::string &instructions, const std::string &input,
                                                         const std::string &jsonSchema) {
    json schema;
    try {
        schema = json::parse(jsonSchema);
    } catch (const json::exception &error) {
        return fail(ErrorCode::Internal, std::string("invalid JSON schema: ") + error.what());
    }
    const json request = {
        {"model", config_.model},
        {"temperature", 0},
        {"messages",
         json::array({{{"role", "system"}, {"content", instructions}}, {{"role", "user"}, {"content", input}}})},
        // Structured output: the server constrains generation to this schema.
        {"response_format", {{"type", "json_schema"}, {"json_schema", {{"name", "receipt"}, {"schema", schema}}}}},
    };
    auto body = post("/v1/chat/completions", request.dump());
    if (!body) {
        return std::unexpected(body.error());
    }
    try {
        return json::parse(*body).at("choices").at(0).at("message").at("content").get<std::string>();
    } catch (const json::exception &error) {
        return fail(ErrorCode::Server, std::string("unexpected /v1/chat/completions response: ") + error.what());
    }
}

} // namespace receipts::inference
