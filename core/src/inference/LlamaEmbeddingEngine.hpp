#pragma once

// EmbeddingGemma (or any GGUF embedding model) running in-process through
// llama.cpp: CPU with NEON on phones, Metal on Macs.
//
// One llama context is reused for every text, so calls are serialised.

#include "inference/EmbeddingEngine.hpp"

#include <memory>
#include <mutex>

struct llama_model;
struct llama_context;

namespace receipts::inference {

class LlamaEmbeddingEngine : public EmbeddingEngine {
public:
    static Result<std::unique_ptr<LlamaEmbeddingEngine>> load(const std::string &modelPath);
    ~LlamaEmbeddingEngine() override;

    Result<std::vector<Embedding>> embed(const std::vector<std::string> &texts) override;
    std::string modelId() const override { return modelId_; }

private:
    LlamaEmbeddingEngine(llama_model *model, llama_context *context, std::string modelId);
    Result<Embedding> embedOne(const std::string &text);

    llama_model *model_;
    llama_context *context_;
    std::string modelId_;
    std::mutex mutex_;
};

} // namespace receipts::inference
