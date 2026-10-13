#pragma once

// Text -> vector. Two implementations:
//   LlamaEmbeddingEngine     the model runs in-process (llama.cpp, GGUF file)
//   OpenAiCompatibleEngine   the model runs on a server (/v1/embeddings)
// Callers (EmbeddingLineScorer) cannot tell them apart.

#include "receipts/Result.hpp"

#include <string>
#include <vector>

namespace receipts::inference {

// Always L2-normalised, so a dot product is the cosine similarity.
using Embedding = std::vector<float>;

class EmbeddingEngine {
public:
    virtual ~EmbeddingEngine() = default;
    virtual Result<std::vector<Embedding>> embed(const std::vector<std::string> &texts) = 0;
    // Identifies the model (e.g. for caching vectors); changes when the model does.
    virtual std::string modelId() const = 0;
};

} // namespace receipts::inference
