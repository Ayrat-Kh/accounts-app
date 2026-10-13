#include "store/LocalReceiptStore.hpp"
#include "support/TempDirectory.hpp"

#include <catch2/catch_test_macros.hpp>

#include <map>

using namespace receipts;
using receipts::store::LocalReceiptStore;

namespace {

ReceiptDraft draft(std::int64_t amountMinor, std::string purchasedAt) {
    return {.money = {amountMinor, "EUR"}, .purchasedAt = std::move(purchasedAt), .merchant = "Bakery",
            .ocrConfidence = 0.9, .imagePath = std::nullopt};
}

Receipt serverReceipt(std::string id, std::int64_t amountMinor) {
    return {.localId = "",
            .serverId = std::move(id),
            .money = {amountMinor, "EUR"},
            .purchasedAt = "2026-10-01T12:00:00.000Z",
            .merchant = std::nullopt,
            .ocrConfidence = std::nullopt,
            .imagePath = std::nullopt,
            .hasImageOnServer = false,
            .syncState = SyncState::Synced};
}

} // namespace

TEST_CASE("a saved receipt is listed and waits for upload") {
    testing::TempDirectory directory;
    auto store = LocalReceiptStore::open(directory.file("receipts.db")).value();

    REQUIRE(store->addPending(draft(1234, "2026-10-13T12:00:00.000Z"), "local-1", std::nullopt));
    const auto visible = store->listVisible().value();

    REQUIRE(visible.size() == 1);
    CHECK(visible[0].money.amountMinor == 1234);
    CHECK(visible[0].syncState == SyncState::PendingUpload);
    CHECK(store->listByState(SyncState::PendingUpload).value().size() == 1);
}

TEST_CASE("receipts are listed newest purchase first") {
    testing::TempDirectory directory;
    auto store = LocalReceiptStore::open(directory.file("receipts.db")).value();
    REQUIRE(store->addPending(draft(1, "2026-10-01T12:00:00.000Z"), "local-old", std::nullopt));
    REQUIRE(store->addPending(draft(2, "2026-10-13T12:00:00.000Z"), "local-new", std::nullopt));

    const auto visible = store->listVisible().value();
    CHECK(visible[0].localId == "local-new");
    CHECK(visible[1].localId == "local-old");
}

TEST_CASE("deleting a never-uploaded receipt forgets it at once") {
    testing::TempDirectory directory;
    auto store = LocalReceiptStore::open(directory.file("receipts.db")).value();
    REQUIRE(store->addPending(draft(1, "2026-10-01T12:00:00.000Z"), "local-1", std::nullopt));

    REQUIRE(store->markPendingDelete("local-1"));
    CHECK_FALSE(store->find("local-1").value());
}

TEST_CASE("deleting an uploaded receipt hides it until the server confirms") {
    testing::TempDirectory directory;
    auto store = LocalReceiptStore::open(directory.file("receipts.db")).value();
    REQUIRE(store->addPending(draft(1, "2026-10-01T12:00:00.000Z"), "local-1", std::nullopt));
    REQUIRE(store->markUploaded("local-1", "server-1"));
    REQUIRE(store->markSynced("local-1"));

    REQUIRE(store->markPendingDelete("local-1"));
    CHECK(store->listVisible().value().empty());
    CHECK(store->listByState(SyncState::PendingDelete).value().size() == 1);
}

TEST_CASE("merging the server's list adds, updates and removes synced receipts") {
    testing::TempDirectory directory;
    auto store = LocalReceiptStore::open(directory.file("receipts.db")).value();
    REQUIRE(store->mergeFromServer({serverReceipt("a", 100), serverReceipt("b", 200)}).value() == 2);

    // "a" changed on another device, "b" was deleted there, "c" is new.
    CHECK(store->mergeFromServer({serverReceipt("a", 150), serverReceipt("c", 300)}).value() == 1);

    const auto visible = store->listVisible().value();
    REQUIRE(visible.size() == 2);
    std::map<std::string, std::int64_t> amounts;
    for (const auto &receipt : visible) {
        amounts[*receipt.serverId] = receipt.money.amountMinor;
    }
    CHECK(amounts == std::map<std::string, std::int64_t>{{"a", 150}, {"c", 300}});
}

TEST_CASE("merging never overwrites a receipt with local changes") {
    testing::TempDirectory directory;
    auto store = LocalReceiptStore::open(directory.file("receipts.db")).value();
    REQUIRE(store->addPending(draft(999, "2026-10-01T12:00:00.000Z"), "local-1", std::nullopt));
    store->markUploaded("local-1", "a").value();  // uploaded, but the photo is still pending

    REQUIRE(store->mergeFromServer({serverReceipt("a", 100)}));
    CHECK(store->find("local-1").value()->money.amountMinor == 999);
}
