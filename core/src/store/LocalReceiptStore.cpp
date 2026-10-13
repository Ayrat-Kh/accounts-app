#include "store/LocalReceiptStore.hpp"

#include <set>

namespace receipts::store {

namespace {

constexpr const char *schema = R"sql(
    CREATE TABLE IF NOT EXISTS receipts (
        local_id            TEXT PRIMARY KEY,
        server_id           TEXT UNIQUE,
        amount_minor        INTEGER NOT NULL,
        currency            TEXT NOT NULL,
        purchased_at        TEXT NOT NULL,   -- RFC 3339 UTC: sorts correctly as text
        merchant            TEXT,
        ocr_confidence      REAL,
        image_path          TEXT,
        has_image_on_server INTEGER NOT NULL DEFAULT 0,
        sync_state          TEXT NOT NULL    -- 'pending_upload' | 'pending_delete' | 'synced'
    );
)sql";

// Column list shared by every SELECT, in the order receiptFromRow reads it.
constexpr const char *receiptColumns = "local_id, server_id, amount_minor, currency, purchased_at, merchant, "
                                       "ocr_confidence, image_path, has_image_on_server, sync_state";

std::string stateName(SyncState state) {
    switch (state) {
    case SyncState::Synced: return "synced";
    case SyncState::PendingUpload: return "pending_upload";
    case SyncState::PendingDelete: return "pending_delete";
    }
    return "pending_upload";
}

SyncState stateFromName(const std::string &name) {
    if (name == "synced")
        return SyncState::Synced;
    if (name == "pending_delete")
        return SyncState::PendingDelete;
    return SyncState::PendingUpload;
}

Receipt receiptFromRow(const Statement &row) {
    return Receipt{
        .localId = row.text(0),
        .serverId = row.optionalText(1),
        .money = {row.int64(2), row.text(3)},
        .purchasedAt = row.text(4),
        .merchant = row.optionalText(5),
        .ocrConfidence = row.optionalDouble(6),
        .imagePath = row.optionalText(7),
        .hasImageOnServer = row.int64(8) != 0,
        .syncState = stateFromName(row.text(9)),
    };
}

std::vector<Receipt> readAll(Statement &statement) {
    std::vector<Receipt> receipts;
    while (statement.step()) {
        receipts.push_back(receiptFromRow(statement));
    }
    return receipts;
}

} // namespace

// SQLite errors here can only come from bugs or a broken file; they are
// thrown by Statement and turned into an Internal error at this boundary.
template <class Function>
auto LocalReceiptStore::locked(Function function) -> decltype(function()) {
    std::lock_guard lock(mutex_);
    try {
        return function();
    } catch (const std::exception &error) {
        return fail(ErrorCode::Internal, std::string("local database: ") + error.what());
    }
}

Result<std::unique_ptr<LocalReceiptStore>> LocalReceiptStore::open(const std::string &databasePath) {
    auto database = Database::open(databasePath);
    if (!database) {
        return std::unexpected(database.error());
    }
    std::unique_ptr<LocalReceiptStore> store(new LocalReceiptStore(std::move(*database)));
    if (auto created = store->locked([&]() -> Result<void> {
            store->createSchema();
            return {};
        });
        !created) {
        return std::unexpected(created.error());
    }
    return store;
}

void LocalReceiptStore::createSchema() {
    database_->execute(schema);
}

Result<Receipt> LocalReceiptStore::addPending(const ReceiptDraft &draft, const std::string &localId,
                                              std::optional<std::string> imagePath) {
    return locked([&]() -> Result<Receipt> {
        auto insert = database_->prepare(
            "INSERT INTO receipts (local_id, amount_minor, currency, purchased_at, merchant, ocr_confidence, "
            "image_path, sync_state) VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, 'pending_upload')");
        insert.bind(1, localId);
        insert.bind(2, draft.money.amountMinor);
        insert.bind(3, draft.money.currency);
        insert.bind(4, draft.purchasedAt);
        insert.bind(5, draft.merchant);
        insert.bind(6, draft.ocrConfidence);
        insert.bind(7, imagePath);
        insert.run();
        return Receipt{.localId = localId,
                       .serverId = std::nullopt,
                       .money = draft.money,
                       .purchasedAt = draft.purchasedAt,
                       .merchant = draft.merchant,
                       .ocrConfidence = draft.ocrConfidence,
                       .imagePath = std::move(imagePath),
                       .hasImageOnServer = false,
                       .syncState = SyncState::PendingUpload};
    });
}

Result<std::optional<Receipt>> LocalReceiptStore::find(const std::string &localId) {
    return locked([&]() -> Result<std::optional<Receipt>> {
        auto select =
            database_->prepare(std::string("SELECT ") + receiptColumns + " FROM receipts WHERE local_id = ?1");
        select.bind(1, localId);
        if (!select.step()) {
            return std::nullopt;
        }
        return receiptFromRow(select);
    });
}

Result<std::vector<Receipt>> LocalReceiptStore::listVisible() {
    return locked([&]() -> Result<std::vector<Receipt>> {
        auto select = database_->prepare(std::string("SELECT ") + receiptColumns +
                                         " FROM receipts WHERE sync_state != 'pending_delete' "
                                         "ORDER BY purchased_at DESC, local_id DESC");
        return readAll(select);
    });
}

Result<std::vector<Receipt>> LocalReceiptStore::listByState(SyncState state) {
    return locked([&]() -> Result<std::vector<Receipt>> {
        auto select = database_->prepare(std::string("SELECT ") + receiptColumns +
                                         " FROM receipts WHERE sync_state = ?1 ORDER BY purchased_at");
        select.bind(1, stateName(state));
        return readAll(select);
    });
}

Result<void> LocalReceiptStore::markUploaded(const std::string &localId, const std::string &serverId) {
    return locked([&]() -> Result<void> {
        auto update = database_->prepare("UPDATE receipts SET server_id = ?2 WHERE local_id = ?1");
        update.bind(1, localId);
        update.bind(2, serverId);
        update.run();
        return {};
    });
}

Result<void> LocalReceiptStore::markImageUploaded(const std::string &localId) {
    return locked([&]() -> Result<void> {
        auto update = database_->prepare("UPDATE receipts SET has_image_on_server = 1 WHERE local_id = ?1");
        update.bind(1, localId);
        update.run();
        return {};
    });
}

Result<void> LocalReceiptStore::markSynced(const std::string &localId) {
    return locked([&]() -> Result<void> {
        auto update = database_->prepare("UPDATE receipts SET sync_state = 'synced' WHERE local_id = ?1");
        update.bind(1, localId);
        update.run();
        return {};
    });
}

Result<void> LocalReceiptStore::markPendingDelete(const std::string &localId) {
    return locked([&]() -> Result<void> {
        // Never uploaded: nothing to tell the server, just forget it.
        auto forget = database_->prepare("DELETE FROM receipts WHERE local_id = ?1 AND server_id IS NULL");
        forget.bind(1, localId);
        forget.run();
        auto mark = database_->prepare("UPDATE receipts SET sync_state = 'pending_delete' WHERE local_id = ?1");
        mark.bind(1, localId);
        mark.run();
        return {};
    });
}

Result<void> LocalReceiptStore::removeLocal(const std::string &localId) {
    return locked([&]() -> Result<void> {
        auto remove = database_->prepare("DELETE FROM receipts WHERE local_id = ?1");
        remove.bind(1, localId);
        remove.run();
        return {};
    });
}

Result<int> LocalReceiptStore::mergeFromServer(const std::vector<Receipt> &serverReceipts) {
    return locked([&]() -> Result<int> {
        database_->execute("BEGIN");
        try {
            int added = 0;
            std::set<std::string> serverIds;
            for (const Receipt &receipt : serverReceipts) {
                serverIds.insert(*receipt.serverId);

                // Refresh rows we already know (unless they have local changes)...
                auto update = database_->prepare(
                    "UPDATE receipts SET amount_minor = ?2, currency = ?3, purchased_at = ?4, merchant = ?5, "
                    "ocr_confidence = ?6, has_image_on_server = ?7 WHERE server_id = ?1 AND sync_state = 'synced'");
                update.bind(1, *receipt.serverId);
                update.bind(2, receipt.money.amountMinor);
                update.bind(3, receipt.money.currency);
                update.bind(4, receipt.purchasedAt);
                update.bind(5, receipt.merchant);
                update.bind(6, receipt.ocrConfidence);
                update.bind(7, static_cast<std::int64_t>(receipt.hasImageOnServer));
                update.run();

                // ...and add the ones created on another device.
                auto insert = database_->prepare(
                    "INSERT OR IGNORE INTO receipts (local_id, server_id, amount_minor, currency, purchased_at, "
                    "merchant, ocr_confidence, has_image_on_server, sync_state) "
                    "VALUES (?1, ?1, ?2, ?3, ?4, ?5, ?6, ?7, 'synced')");
                insert.bind(1, *receipt.serverId);
                insert.bind(2, receipt.money.amountMinor);
                insert.bind(3, receipt.money.currency);
                insert.bind(4, receipt.purchasedAt);
                insert.bind(5, receipt.merchant);
                insert.bind(6, receipt.ocrConfidence);
                insert.bind(7, static_cast<std::int64_t>(receipt.hasImageOnServer));
                insert.run();
                added += database_->changes();
            }

            // Synced rows the server no longer has were deleted elsewhere.
            auto synced = database_->prepare("SELECT local_id, server_id FROM receipts WHERE sync_state = 'synced'");
            std::vector<std::string> gone;
            while (synced.step()) {
                if (!serverIds.contains(synced.text(1))) {
                    gone.push_back(synced.text(0));
                }
            }
            for (const std::string &localId : gone) {
                auto remove = database_->prepare("DELETE FROM receipts WHERE local_id = ?1");
                remove.bind(1, localId);
                remove.run();
            }

            database_->execute("COMMIT");
            return added;
        } catch (...) {
            database_->execute("ROLLBACK");
            throw;
        }
    });
}

} // namespace receipts::store
