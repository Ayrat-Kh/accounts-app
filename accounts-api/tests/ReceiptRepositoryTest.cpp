// Integration tests against a real Postgres (see support/TestDatabase.hpp).

#include "receipts/ReceiptRepository.hpp"
#include "support/TestDatabase.hpp"
#include "support/TestSupport.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace accounts;
using namespace accounts::receipts;
using testing::runSync;

namespace {

ReceiptDraft draftAt(const std::string &purchasedAt, std::int64_t amountMinor = 1999) {
    return {.amountMinor = amountMinor, .currency = "EUR", .purchasedAt = parseRfc3339(purchasedAt).value(),
            .merchant = "Bakery", .ocrConfidence = 0.87};
}

} // namespace

TEST_CASE("a created receipt can be read back by its owner") {
    ReceiptRepository repository(testing::testDatabase());
    const auto owner = testing::createTestUser();

    const Receipt created = runSync(repository.create(owner.id, draftAt("2026-10-01T12:00:00Z"))).value();
    const auto found = runSync(repository.find(owner.id, created.id)).value();

    REQUIRE(found);
    CHECK(found->amountMinor == 1999);
    CHECK(found->currency == "EUR");
    CHECK(found->merchant == "Bakery");
    CHECK(formatRfc3339(found->purchasedAt) == "2026-10-01T12:00:00.000Z");
    CHECK_FALSE(found->image);
}

TEST_CASE("another user can neither read, change nor delete a receipt") {
    // The old API let anyone overwrite any account by id (IDOR).
    ReceiptRepository repository(testing::testDatabase());
    const auto owner = testing::createTestUser();
    const auto stranger = testing::createTestUser();
    const Receipt receipt = runSync(repository.create(owner.id, draftAt("2026-10-01T12:00:00Z"))).value();

    CHECK_FALSE(runSync(repository.find(stranger.id, receipt.id)).value());
    CHECK_FALSE(runSync(repository.update(stranger.id, receipt.id, {.amountMinor = 1})).value());
    CHECK_FALSE(runSync(repository.remove(stranger.id, receipt.id)).value());

    CHECK(runSync(repository.find(owner.id, receipt.id)).value()->amountMinor == 1999);
}

TEST_CASE("a partial update changes only the given fields; an empty merchant clears it") {
    ReceiptRepository repository(testing::testDatabase());
    const auto owner = testing::createTestUser();
    const Receipt receipt = runSync(repository.create(owner.id, draftAt("2026-10-01T12:00:00Z"))).value();

    const auto updated =
        runSync(repository.update(owner.id, receipt.id, {.amountMinor = 2500, .merchant = std::string()})).value();

    REQUIRE(updated);
    CHECK(updated->amountMinor == 2500);
    CHECK(updated->currency == "EUR");
    CHECK_FALSE(updated->merchant);
}

TEST_CASE("a deleted receipt disappears") {
    ReceiptRepository repository(testing::testDatabase());
    const auto owner = testing::createTestUser();
    const Receipt receipt = runSync(repository.create(owner.id, draftAt("2026-10-01T12:00:00Z"))).value();

    CHECK(runSync(repository.remove(owner.id, receipt.id)).value());
    CHECK_FALSE(runSync(repository.find(owner.id, receipt.id)).value());
    CHECK_FALSE(runSync(repository.remove(owner.id, receipt.id)).value());
}

TEST_CASE("listing pages through receipts newest first without gaps or repeats") {
    ReceiptRepository repository(testing::testDatabase());
    const auto owner = testing::createTestUser();
    for (const char *day : {"2026-10-01", "2026-10-02", "2026-10-03", "2026-10-04", "2026-10-05"}) {
        runSync(repository.create(owner.id, draftAt(std::string(day) + "T08:00:00Z"))).value();
    }

    const auto firstPage = runSync(repository.list(owner.id, std::nullopt, 2)).value();
    REQUIRE(firstPage.size() == 2);
    CHECK(formatRfc3339(firstPage[0].purchasedAt).starts_with("2026-10-05"));
    CHECK(formatRfc3339(firstPage[1].purchasedAt).starts_with("2026-10-04"));

    const PageCursor afterSecond{firstPage[1].purchasedAt, firstPage[1].id};
    const auto rest = runSync(repository.list(owner.id, afterSecond, 10)).value();
    REQUIRE(rest.size() == 3);
    CHECK(formatRfc3339(rest[0].purchasedAt).starts_with("2026-10-03"));
    CHECK(formatRfc3339(rest[2].purchasedAt).starts_with("2026-10-01"));
}
