#include "receipts/ReceiptRepository.hpp"

namespace accounts::receipts {

using boost::asio::awaitable;
using db::PgResult;
using db::PgRow;

namespace {

// The columns every query returns, in the shape receiptFromRow() reads.
// Timestamps come back as RFC 3339 text (see rfc3339() in 001_init.sql).
const std::string receiptColumns = R"sql(
    id, user_id, amount_minor, currency, merchant, ocr_confidence,
    rfc3339(purchased_at) AS purchased_at,
    rfc3339(created_at) AS created_at,
    rfc3339(updated_at) AS updated_at,
    image_object_key, image_sha256, image_size_bytes, image_mime_type
)sql";

Timestamp timestampColumn(const PgRow &row, std::string_view column) {
    return parseRfc3339(row.text(column)).value();  // the database always sends valid ones
}

Receipt receiptFromRow(const PgRow &row) {
    Receipt receipt{
        .id = row.text("id"),
        .userId = row.text("user_id"),
        .amountMinor = row.int64("amount_minor"),
        .currency = row.text("currency"),
        .purchasedAt = timestampColumn(row, "purchased_at"),
        .merchant = row.optionalText("merchant"),
        .ocrConfidence = row.optionalDouble("ocr_confidence"),
        .image = std::nullopt,
        .createdAt = timestampColumn(row, "created_at"),
        .updatedAt = timestampColumn(row, "updated_at"),
    };
    if (!row.isNull("image_object_key")) {
        receipt.image = ReceiptImage{
            .objectKey = row.text("image_object_key"),
            .sha256 = row.text("image_sha256"),
            .sizeBytes = row.int64("image_size_bytes"),
            .mimeType = row.text("image_mime_type"),
        };
    }
    return receipt;
}

Result<std::vector<Receipt>> allReceipts(const Result<PgResult> &result) {
    if (!result) {
        return std::unexpected(result.error());
    }
    std::vector<Receipt> receipts;
    receipts.reserve(result->rowCount());
    for (const PgRow row : result->rows()) {
        receipts.push_back(receiptFromRow(row));
    }
    return receipts;
}

Result<std::optional<Receipt>> optionalReceipt(const Result<PgResult> &result) {
    if (!result) {
        return std::unexpected(result.error());
    }
    if (result->empty()) {
        return std::nullopt;
    }
    return receiptFromRow(result->firstRow());
}

} // namespace

awaitable<Result<Receipt>> ReceiptRepository::create(const std::string &userId, const ReceiptDraft &draft) {
    auto result = co_await pool_.query(R"sql(
        INSERT INTO receipts (user_id, amount_minor, currency, purchased_at, merchant, ocr_confidence)
        VALUES ($1, $2, $3, $4, $5, $6)
        RETURNING )sql" + receiptColumns,
                                       userId, draft.amountMinor, draft.currency, draft.purchasedAt, draft.merchant,
                                       draft.ocrConfidence);
    auto created = optionalReceipt(result);
    if (!created) {
        co_return std::unexpected(created.error());
    }
    co_return created->value();  // INSERT ... RETURNING always yields the row
}

awaitable<Result<std::optional<Receipt>>> ReceiptRepository::find(const std::string &userId, const std::string &id) {
    co_return optionalReceipt(co_await pool_.query(
        "SELECT " + receiptColumns + " FROM receipts WHERE id = $1 AND user_id = $2 AND deleted_at IS NULL", id,
        userId));
}

awaitable<Result<std::vector<Receipt>>> ReceiptRepository::list(const std::string &userId,
                                                                const std::optional<PageCursor> &after, int limit) {
    // Row comparison (a, b) < (x, y) matches the ORDER BY exactly and uses
    // receipts_user_purchased_idx. $2/$3 are NULL for the first page.
    co_return allReceipts(co_await pool_.query(
        "SELECT " + receiptColumns + R"sql(
        FROM receipts
        WHERE user_id = $1
          AND deleted_at IS NULL
          AND ($2::timestamptz IS NULL OR (purchased_at, id) < ($2::timestamptz, $3::uuid))
        ORDER BY purchased_at DESC, id DESC
        LIMIT $4
    )sql",
        userId, after ? std::optional(after->purchasedAt) : std::nullopt,
        after ? std::optional(after->id) : std::nullopt, limit));
}

awaitable<Result<std::optional<Receipt>>> ReceiptRepository::update(const std::string &userId, const std::string &id,
                                                                    const ReceiptChanges &changes) {
    // COALESCE keeps the current value for every field the client left out;
    // NULLIF turns an explicit "" merchant into NULL.
    co_return optionalReceipt(co_await pool_.query(R"sql(
        UPDATE receipts SET
            amount_minor   = COALESCE($3, amount_minor),
            currency       = COALESCE($4, currency),
            purchased_at   = COALESCE($5::timestamptz, purchased_at),
            merchant       = NULLIF(COALESCE($6, merchant), ''),
            ocr_confidence = COALESCE($7, ocr_confidence),
            updated_at     = now(),
            obsidian_synced_at = NULL
        WHERE id = $1 AND user_id = $2 AND deleted_at IS NULL
        RETURNING )sql" + receiptColumns,
                                                   id, userId, changes.amountMinor, changes.currency,
                                                   changes.purchasedAt, changes.merchant, changes.ocrConfidence));
}

awaitable<Result<bool>> ReceiptRepository::remove(const std::string &userId, const std::string &id) {
    auto result = co_await pool_.query(R"sql(
        UPDATE receipts SET deleted_at = now()
        WHERE id = $1 AND user_id = $2 AND deleted_at IS NULL
        RETURNING id
    )sql",
                                       id, userId);
    if (!result) {
        co_return std::unexpected(result.error());
    }
    co_return !result->empty();
}

awaitable<Result<std::optional<Receipt>>> ReceiptRepository::attachImage(const std::string &userId,
                                                                         const std::string &id,
                                                                         const ReceiptImage &image) {
    co_return optionalReceipt(co_await pool_.query(R"sql(
        UPDATE receipts SET
            image_object_key = $3,
            image_sha256     = $4,
            image_size_bytes = $5,
            image_mime_type  = $6,
            updated_at       = now()
        WHERE id = $1 AND user_id = $2 AND deleted_at IS NULL
        RETURNING )sql" + receiptColumns,
                                                   id, userId, image.objectKey, image.sha256, image.sizeBytes,
                                                   image.mimeType));
}

awaitable<Result<std::vector<Receipt>>> ReceiptRepository::listNotYetInObsidian(int limit) {
    co_return allReceipts(co_await pool_.query("SELECT " + receiptColumns + R"sql(
        FROM receipts
        WHERE obsidian_synced_at IS NULL AND deleted_at IS NULL
        ORDER BY created_at
        LIMIT $1
    )sql",
                                               limit));
}

awaitable<Result<void>> ReceiptRepository::markInObsidian(const std::string &id) {
    co_return co_await pool_.command("UPDATE receipts SET obsidian_synced_at = now() WHERE id = $1", id);
}

} // namespace accounts::receipts
