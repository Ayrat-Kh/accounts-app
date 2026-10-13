#pragma once

// The device's own copy of the receipts, and the outbox of changes the
// server hasn't seen yet. The UI always reads from here, so it works
// offline; SyncEngine reconciles with the server when it can.
//
// Lifecycle of a row:
//   addPending()            PendingUpload, no server id
//   markUploaded()          PendingUpload, has a server id (the photo may still be pending)
//   markSynced()            Synced
//   markPendingDelete()     PendingDelete (or deleted at once if never uploaded)
//   removeLocal()           gone

#include "receipts/Result.hpp"
#include "receipts/Types.hpp"
#include "store/Sqlite.hpp"

#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

namespace receipts::store {

class LocalReceiptStore {
public:
    static Result<std::unique_ptr<LocalReceiptStore>> open(const std::string &databasePath);

    // `imagePath` is where the photo now lives inside the data directory.
    Result<Receipt> addPending(const ReceiptDraft &draft, const std::string &localId,
                               std::optional<std::string> imagePath);
    Result<std::optional<Receipt>> find(const std::string &localId);
    // Everything the UI shows: all receipts except those waiting to be deleted, newest first.
    Result<std::vector<Receipt>> listVisible();
    Result<std::vector<Receipt>> listByState(SyncState state);

    Result<void> markUploaded(const std::string &localId, const std::string &serverId);
    Result<void> markImageUploaded(const std::string &localId);
    Result<void> markSynced(const std::string &localId);
    Result<void> markPendingDelete(const std::string &localId);
    Result<void> removeLocal(const std::string &localId);

    // Makes the synced rows match the server's list: adds new receipts,
    // updates changed ones, removes those deleted elsewhere. Rows with
    // pending local changes are left alone. Returns how many were added.
    Result<int> mergeFromServer(const std::vector<Receipt> &serverReceipts);

private:
    explicit LocalReceiptStore(std::unique_ptr<Database> database) : database_(std::move(database)) {}
    void createSchema();

    // Runs `function` under the store's lock and turns SQLite exceptions
    // into an Internal error. The lock keeps one caller's transaction from
    // interleaving with another thread's statements.
    template <class Function>
    auto locked(Function function) -> decltype(function());

    std::unique_ptr<Database> database_;
    std::mutex mutex_;
};

} // namespace receipts::store
