#pragma once

// A model behind an OpenAI-compatible HTTP API, the de-facto standard spoken by
// llama.cpp's llama-server, oMLX and other MLX servers, Ollama and LM Studio:
//   POST {baseUrl}/v1/embeddings        {"model", "input": [...]}
//   POST {baseUrl}/v1/chat/completions  {"model", "messages", "response_format": json_schema}
//
// Useful on a Mac, where a bigger model on Apple silicon can be far more
// accurate than what fits on a phone.

#include "inference/CompletionEngine.hpp"
#include "inference/EmbeddingEngine.hpp"
#include "net/HttpTransport.hpp"
#include "receipts/Config.hpp"

#include <memory>
#include <optional>

namespace receipts::inference {

class OpenAiCompatibleEngine : public EmbeddingEngine, public CompletionEngine {
public:
    OpenAiCompatibleEngine(std::shared_ptr<net::HttpTransport> transport, EngineConfig config,
                           std::optional<std::string> apiKey);

    Result<std::vector<Embedding>> embed(const std::vector<std::string> &texts) override;
    std::string modelId() const override;

    Result<std::string> completeJson(const std::string &instructions, const std::string &input,
                                     const std::string &jsonSchema) override;

private:
    Result<std::string> post(const std::string &path, const std::string &body);

    std::shared_ptr<net::HttpTransport> transport_;
    EngineConfig config_;
    std::optional<std::string> apiKey_;
};

} // namespace receipts::inference
