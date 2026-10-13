#include "receipts/Receipt.hpp"

#include "util/Crypto.hpp"

#include <algorithm>
#include <cctype>

namespace accounts::receipts {

namespace {

// Ten billion in major units is more than any receipt; the bound catches
// unit mix-ups (sending cents twice) and overflow when summing.
constexpr std::int64_t maxAmountMinor = 1'000'000'000'000;
constexpr std::size_t maxMerchantLength = 200;

Result<void> validateAmount(std::int64_t amountMinor) {
    if (amountMinor < 0 || amountMinor > maxAmountMinor) {
        return fail(ErrorCode::InvalidInput, "amount_minor must be between 0 and 10^12");
    }
    return {};
}

Result<void> validateCurrency(std::string_view currency) {
    const bool isThreeUpperCaseLetters =
        currency.size() == 3 && std::ranges::all_of(currency, [](char c) { return c >= 'A' && c <= 'Z'; });
    if (!isThreeUpperCaseLetters) {
        return fail(ErrorCode::InvalidInput, "currency must be an ISO 4217 code like \"EUR\"");
    }
    return {};
}

Result<void> validateMerchant(const std::string &merchant) {
    if (merchant.size() > maxMerchantLength) {
        return fail(ErrorCode::InvalidInput, "merchant must be at most 200 characters");
    }
    return {};
}

Result<void> validateConfidence(double confidence) {
    if (!(confidence >= 0.0 && confidence <= 1.0)) {  // also rejects NaN
        return fail(ErrorCode::InvalidInput, "ocr_confidence must be between 0 and 1");
    }
    return {};
}

// Runs the checks in order and returns the first failure.
Result<void> firstFailure(std::initializer_list<Result<void>> checks) {
    for (const auto &check : checks) {
        if (!check) {
            return check;
        }
    }
    return {};
}

} // namespace

Result<void> validate(const ReceiptDraft &draft) {
    return firstFailure({
        validateAmount(draft.amountMinor),
        validateCurrency(draft.currency),
        draft.merchant ? validateMerchant(*draft.merchant) : Result<void>{},
        draft.ocrConfidence ? validateConfidence(*draft.ocrConfidence) : Result<void>{},
    });
}

Result<void> validate(const ReceiptChanges &changes) {
    return firstFailure({
        changes.amountMinor ? validateAmount(*changes.amountMinor) : Result<void>{},
        changes.currency ? validateCurrency(*changes.currency) : Result<void>{},
        changes.merchant ? validateMerchant(*changes.merchant) : Result<void>{},
        changes.ocrConfidence ? validateConfidence(*changes.ocrConfidence) : Result<void>{},
    });
}

bool isUuid(std::string_view text) {
    // 8-4-4-4-12 hex digits
    if (text.size() != 36) {
        return false;
    }
    for (std::size_t i = 0; i < text.size(); ++i) {
        const bool isDashPosition = i == 8 || i == 13 || i == 18 || i == 23;
        const bool valid = isDashPosition ? text[i] == '-' : std::isxdigit(static_cast<unsigned char>(text[i])) != 0;
        if (!valid) {
            return false;
        }
    }
    return true;
}

std::string encodeCursor(const PageCursor &cursor) {
    // Opaque to clients: "<rfc3339>|<id>" in base64url.
    return crypto::base64UrlEncode(formatRfc3339(cursor.purchasedAt) + "|" + cursor.id);
}

std::optional<PageCursor> decodeCursor(std::string_view encoded) {
    const std::string decoded = crypto::base64UrlDecode(encoded);
    const auto separator = decoded.find('|');
    if (separator == std::string::npos) {
        return std::nullopt;
    }
    const auto purchasedAt = parseRfc3339(std::string_view(decoded).substr(0, separator));
    std::string id = decoded.substr(separator + 1);
    if (!purchasedAt || !isUuid(id)) {
        return std::nullopt;
    }
    return PageCursor{*purchasedAt, std::move(id)};
}

} // namespace accounts::receipts
