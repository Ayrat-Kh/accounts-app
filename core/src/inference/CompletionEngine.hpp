#pragma once

// A generative model that answers in JSON matching a schema. Optional; used
// only when the embedding-based extraction is unsure (see LlmTotalExtractor).

#include "receipts/Result.hpp"

#include <string>

namespace receipts::inference {

class CompletionEngine {
public:
    virtual ~CompletionEngine() = default;
    // Returns the model's raw JSON text; the caller validates it.
    virtual Result<std::string> completeJson(const std::string &instructions, const std::string &input,
                                             const std::string &jsonSchema) = 0;
};

} // namespace receipts::inference
