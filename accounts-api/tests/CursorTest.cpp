#include "receipts/Receipt.hpp"
#include "util/Crypto.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace accounts::receipts;

TEST_CASE("a page cursor survives an encode/decode round trip") {
    const PageCursor cursor{accounts::parseRfc3339("2026-10-13T09:30:00.250Z").value(),
                            "0b7e7c9a-2f4e-4b8e-9b1a-3c2d1e0f9a8b"};
    const auto decoded = decodeCursor(encodeCursor(cursor));
    REQUIRE(decoded);
    CHECK(decoded->purchasedAt == cursor.purchasedAt);
    CHECK(decoded->id == cursor.id);
}

TEST_CASE("garbage cursors are rejected instead of reaching SQL") {
    CHECK_FALSE(decodeCursor(""));
    CHECK_FALSE(decodeCursor("!!!"));
    CHECK_FALSE(decodeCursor(accounts::crypto::base64UrlEncode("2026-10-13T09:30:00Z|not-a-uuid")));
}
