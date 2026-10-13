#pragma once

// Second opinion from a generative model, for receipts where the line
// scoring was unsure. The model reads the OCR text and names the total.
//
// Generative models can invent numbers, so the answer is accepted only if
// it equals an amount that is actually printed on the receipt.

#include "inference/CompletionEngine.hpp"
#include "receipts/Types.hpp"

#include <optional>
#include <string>
#include <vector>

namespace receipts::extract {

class LlmTotalExtractor {
public:
    explicit LlmTotalExtractor(inference::CompletionEngine &engine) : engine_(engine) {}

    // nullopt when the model's answer is missing, malformed or not on the receipt.
    Result<std::optional<Money>> extractTotal(const std::vector<std::string> &ocrLines,
                                              const std::vector<Money> &printedAmounts);

private:
    inference::CompletionEngine &engine_;
};

// "12.34" -> 1234. Exposed for tests.
std::optional<std::int64_t> parseDecimalAmount(const std::string &text);

} // namespace receipts::extract
