#include "receipts/ReceiptJson.hpp"

namespace accounts::receipts {

namespace {

template <class T>
boost::json::value optionalToJson(const std::optional<T> &value) {
    return value ? boost::json::value(*value) : boost::json::value(nullptr);
}

// Reads an optional RFC 3339 field; a present-but-unparseable value is an error.
std::optional<Timestamp> readTimestamp(http::JsonFields &fields, std::string_view key,
                                       std::optional<AppError> &error) {
    const auto text = fields.optionalString(key);
    if (!text) {
        return std::nullopt;
    }
    auto parsed = parseRfc3339(*text);
    if (!parsed && !error) {
        error = AppError{ErrorCode::InvalidInput, "field '" + std::string(key) + "' must be an RFC 3339 timestamp"};
    }
    return parsed;
}

} // namespace

boost::json::value toJson(const Receipt &receipt) {
    boost::json::value image = nullptr;
    if (receipt.image) {
        image = {
            {"sha256", receipt.image->sha256},
            {"size_bytes", receipt.image->sizeBytes},
            {"mime_type", receipt.image->mimeType},
        };
    }
    return {
        {"id", receipt.id},
        {"amount_minor", receipt.amountMinor},
        {"currency", receipt.currency},
        {"purchased_at", formatRfc3339(receipt.purchasedAt)},
        {"merchant", optionalToJson(receipt.merchant)},
        {"ocr_confidence", optionalToJson(receipt.ocrConfidence)},
        {"image", image},
        {"created_at", formatRfc3339(receipt.createdAt)},
        {"updated_at", formatRfc3339(receipt.updatedAt)},
    };
}

boost::json::value toJson(const ReceiptPage &page) {
    boost::json::array items;
    for (const Receipt &receipt : page.items) {
        items.push_back(toJson(receipt));
    }
    return {{"items", items}, {"next_cursor", optionalToJson(page.nextCursor)}};
}

Result<ReceiptDraft> draftFromJson(const boost::json::object &body) {
    http::JsonFields fields(body);
    std::optional<AppError> timestampError;
    ReceiptDraft draft{
        .amountMinor = fields.requiredInt64("amount_minor"),
        .currency = fields.requiredString("currency"),
        .purchasedAt = {},
        .merchant = fields.optionalString("merchant"),
        .ocrConfidence = fields.optionalDouble("ocr_confidence"),
    };
    const auto purchasedAt = readTimestamp(fields, "purchased_at", timestampError);

    if (const auto &error = fields.firstError()) {
        return std::unexpected(*error);
    }
    if (timestampError) {
        return std::unexpected(*timestampError);
    }
    if (!purchasedAt) {
        return fail(ErrorCode::InvalidInput, "field 'purchased_at' is required");
    }
    draft.purchasedAt = *purchasedAt;
    return draft;
}

Result<ReceiptChanges> changesFromJson(const boost::json::object &body) {
    http::JsonFields fields(body);
    std::optional<AppError> timestampError;
    ReceiptChanges changes{
        .amountMinor = fields.optionalInt64("amount_minor"),
        .currency = fields.optionalString("currency"),
        .purchasedAt = readTimestamp(fields, "purchased_at", timestampError),
        .merchant = fields.optionalString("merchant"),
        .ocrConfidence = fields.optionalDouble("ocr_confidence"),
    };
    if (const auto &error = fields.firstError()) {
        return std::unexpected(*error);
    }
    if (timestampError) {
        return std::unexpected(*timestampError);
    }
    return changes;
}

} // namespace accounts::receipts
