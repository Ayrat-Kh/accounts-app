#include "extract/LlmTotalExtractor.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cstdio>

namespace receipts::extract {

namespace {

constexpr const char *instructions =
    "You read the OCR text of a shop receipt. Reply with the final total the customer paid "
    "(not a subtotal, tax, or change), written with a dot and two decimals, e.g. \"12.34\", "
    "and its ISO 4217 currency code. Use null for anything you cannot find.";

constexpr const char *answerSchema = R"({
    "type": "object",
    "properties": {
        "total": {"type": ["string", "null"]},
        "currency": {"type": ["string", "null"], "pattern": "^[A-Z]{3}$"}
    },
    "required": ["total", "currency"],
    "additionalProperties": false
})";

std::string joinLines(const std::vector<std::string> &lines) {
    std::string joined;
    for (const std::string &line : lines) {
        joined += line;
        joined += '\n';
    }
    return joined;
}

} // namespace

std::optional<std::int64_t> parseDecimalAmount(const std::string &text) {
    long long whole = 0;
    int cents = 0;
    char trailing = 0;
    if (std::sscanf(text.c_str(), "%lld.%2d%c", &whole, &cents, &trailing) == 2 && whole >= 0 &&
        text.find('.') == text.size() - 3) {
        return whole * 100 + cents;
    }
    if (std::sscanf(text.c_str(), "%lld%c", &whole, &trailing) == 1 && whole >= 0) {
        return whole * 100;
    }
    return std::nullopt;
}

Result<std::optional<Money>> LlmTotalExtractor::extractTotal(const std::vector<std::string> &ocrLines,
                                                             const std::vector<Money> &printedAmounts) {
    auto reply = engine_.completeJson(instructions, joinLines(ocrLines), answerSchema);
    if (!reply) {
        return std::unexpected(reply.error());
    }

    const auto answer = nlohmann::json::parse(*reply, nullptr, /*allow_exceptions=*/false);
    if (!answer.is_object() || !answer.contains("total") || !answer["total"].is_string()) {
        return std::nullopt;
    }
    const auto amount = parseDecimalAmount(answer["total"].get<std::string>());
    if (!amount) {
        return std::nullopt;
    }

    // The anti-hallucination rule: only amounts printed on the receipt count.
    const auto printed =
        std::ranges::find_if(printedAmounts, [&](const Money &money) { return money.amountMinor == *amount; });
    if (printed == printedAmounts.end()) {
        return std::nullopt;
    }
    Money total = *printed;
    if (total.currency.empty() && answer.contains("currency") && answer["currency"].is_string()) {
        total.currency = answer["currency"].get<std::string>();
    }
    return total;
}

} // namespace receipts::extract
