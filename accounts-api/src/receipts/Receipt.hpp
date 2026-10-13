#pragma once

// The receipt domain: what we store about a scanned receipt, and the rules a
// receipt must satisfy. No I/O here: the repository, service and routes build on it.

#include "util/Error.hpp"
#include "util/Time.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace accounts::receipts {

struct ReceiptImage {
    std::string objectKey;  // in the object store
    std::string sha256;     // hex
    std::int64_t sizeBytes;
    std::string mimeType;
};

struct Receipt {
    std::string id;
    std::string userId;
    // Money is an integer count of the currency's minor unit (12.34 EUR -> 1234),
    // so sums are exact. Floating point cannot represent 0.10 exactly.
    std::int64_t amountMinor;
    std::string currency;  // ISO 4217, e.g. "EUR"
    Timestamp purchasedAt;
    std::optional<std::string> merchant;
    std::optional<double> ocrConfidence;  // 0..1, how sure the on-device scanner was
    std::optional<ReceiptImage> image;
    Timestamp createdAt;
    Timestamp updatedAt;
};

// What a client sends to create a receipt.
struct ReceiptDraft {
    std::int64_t amountMinor = 0;
    std::string currency;
    Timestamp purchasedAt;
    std::optional<std::string> merchant;
    std::optional<double> ocrConfidence;
};

// What a client sends to edit one: only the fields present change.
// An empty merchant string clears the merchant.
struct ReceiptChanges {
    std::optional<std::int64_t> amountMinor;
    std::optional<std::string> currency;
    std::optional<Timestamp> purchasedAt;
    std::optional<std::string> merchant;
    std::optional<double> ocrConfidence;
};

Result<void> validate(const ReceiptDraft &draft);
Result<void> validate(const ReceiptChanges &changes);

// Receipt ids are UUIDs; anything else cannot exist, so it is answered with
// 404 without asking the database.
bool isUuid(std::string_view text);

// Keyset pagination: a page ends at a (purchasedAt, id) position, and the
// next page starts strictly after it. Unlike OFFSET, this stays correct and
// fast while receipts are being added.
struct PageCursor {
    Timestamp purchasedAt;
    std::string id;
};

std::string encodeCursor(const PageCursor &cursor);
std::optional<PageCursor> decodeCursor(std::string_view encoded);

struct ReceiptPage {
    std::vector<Receipt> items;
    std::optional<std::string> nextCursor;
};

} // namespace accounts::receipts
