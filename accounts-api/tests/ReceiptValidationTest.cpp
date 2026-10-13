#include "receipts/ImageFormat.hpp"
#include "receipts/Receipt.hpp"

#include <catch2/catch_test_macros.hpp>

#include <cmath>

using namespace accounts::receipts;

namespace {

ReceiptDraft validDraft() {
    return {.amountMinor = 1234,
            .currency = "EUR",
            .purchasedAt = accounts::now(),
            .merchant = "Lidl",
            .ocrConfidence = 0.9};
}

} // namespace

TEST_CASE("a well-formed receipt draft is valid") {
    CHECK(validate(validDraft()));
}

TEST_CASE("amounts must be non-negative and bounded") {
    auto draft = validDraft();
    draft.amountMinor = -1;
    CHECK_FALSE(validate(draft));
    draft.amountMinor = 1'000'000'000'001;
    CHECK_FALSE(validate(draft));
}

TEST_CASE("currencies must be three upper-case letters") {
    for (const char *currency : {"eur", "EU", "EURO", "E1R", ""}) {
        auto draft = validDraft();
        draft.currency = currency;
        CHECK_FALSE(validate(draft));
    }
}

TEST_CASE("OCR confidence must be between 0 and 1") {
    auto draft = validDraft();
    draft.ocrConfidence = 1.5;
    CHECK_FALSE(validate(draft));
    draft.ocrConfidence = std::nan("");
    CHECK_FALSE(validate(draft));
}

TEST_CASE("partial changes validate only the fields that are present") {
    CHECK(validate(ReceiptChanges{}));
    CHECK_FALSE(validate(
        ReceiptChanges{.amountMinor = {}, .currency = "usd", .purchasedAt = {}, .merchant = {}, .ocrConfidence = {}}));
}

TEST_CASE("isUuid accepts only canonical UUIDs") {
    CHECK(isUuid("0b7e7c9a-2f4e-4b8e-9b1a-3c2d1e0f9a8b"));
    CHECK_FALSE(isUuid("me"));
    CHECK_FALSE(isUuid("0b7e7c9a2f4e4b8e9b1a3c2d1e0f9a8b"));
    CHECK_FALSE(isUuid("0b7e7c9a-2f4e-4b8e-9b1a-3c2d1e0f9a8g"));
}

TEST_CASE("image formats are recognised by their magic bytes, not by name") {
    CHECK(detectImageMimeType("\xFF\xD8\xFF\xE0 rest of a jpeg") == "image/jpeg");
    CHECK(detectImageMimeType("\x89PNG\r\n\x1A\n rest of a png") == "image/png");
    using namespace std::string_view_literals; // "..."sv keeps the embedded NUL bytes
    CHECK(detectImageMimeType("RIFF\x10\x00\x00\x00WEBPVP8 "sv) == "image/webp");
    CHECK_FALSE(detectImageMimeType("<html>definitely not an image"));
}
