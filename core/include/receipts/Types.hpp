#pragma once

// Plain value types shared by every host (Android, macOS, the CLI).

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace receipts {

// An amount of money in the currency's minor unit: 12.34 EUR is {1234, "EUR"}.
// Integers keep sums exact; floating point cannot represent 0.10.
struct Money {
    std::int64_t amountMinor = 0;
    std::string currency;  // ISO 4217, e.g. "EUR"; empty when the receipt didn't say
};

// Which part of the pipeline produced a scan's total.
enum class ExtractionSource {
    Embedding,  // OCR + EmbeddingGemma line scoring (the default)
    Keywords,   // OCR + keyword matching (no embedding model available)
    Llm,        // OCR + a generative model, used when the others were unsure
};

// One amount found on the receipt, with how likely it is to be the total.
struct AmountCandidate {
    Money money;
    std::string lineText;
    double score = 0;  // higher is more likely; comparable within one scan only
};

struct ScanResult {
    std::optional<Money> total;
    std::optional<std::string> purchasedAt;   // RFC 3339; the printed date at noon UTC
    std::optional<std::string> merchant;
    double confidence = 0;                    // 0..1: how sure we are about `total`
    ExtractionSource source = ExtractionSource::Keywords;
    std::vector<AmountCandidate> candidates;  // best first; lets the UI offer alternatives
    std::vector<std::string> ocrLines;        // for debugging and the "show text" view
};

enum class SyncState {
    Synced,         // the server has it
    PendingUpload,  // saved on the device, not yet on the server
    PendingDelete,  // deleted on the device, not yet on the server
};

struct Receipt {
    std::string localId;                 // stable on this device
    std::optional<std::string> serverId; // set once uploaded
    Money money;
    std::string purchasedAt;             // RFC 3339 UTC, e.g. "2026-10-13T09:30:00.000Z"
    std::optional<std::string> merchant;
    std::optional<double> ocrConfidence;
    std::optional<std::string> imagePath;  // a local copy of the photo, if this device has one
    bool hasImageOnServer = false;
    SyncState syncState = SyncState::PendingUpload;
};

// What the UI saves after the user reviewed a scan.
struct ReceiptDraft {
    Money money;
    std::string purchasedAt;  // RFC 3339
    std::optional<std::string> merchant;
    std::optional<double> ocrConfidence;
    std::optional<std::string> imagePath;  // copied into the library's data directory
};

struct SyncReport {
    int uploaded = 0;
    int deleted = 0;
    int downloaded = 0;
    int failed = 0;
};

} // namespace receipts
