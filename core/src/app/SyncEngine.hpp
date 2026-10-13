#pragma once

// Reconciles the local store with the server, in three steps:
//   1. push: create pending receipts on the server, then upload their photos
//   2. push: delete what was deleted on this device
//   3. pull: replace the synced rows with the server's list
//
// A network error stops the sync at once (we're offline; try later). A
// server rejecting one receipt is counted as failed, and the rest go on.

#include "api/ApiClient.hpp"
#include "store/LocalReceiptStore.hpp"

namespace receipts::app {

class SyncEngine {
public:
    SyncEngine(api::ApiClient &api, store::LocalReceiptStore &store, Logger &logger)
        : api_(api), store_(store), logger_(logger) {}

    Result<SyncReport> sync();

private:
    Result<void> pushUploads(SyncReport &report);
    Result<void> pushDeletes(SyncReport &report);
    Result<void> pull(SyncReport &report);
    Result<void> upload(Receipt receipt);

    api::ApiClient &api_;
    store::LocalReceiptStore &store_;
    Logger &logger_;
};

} // namespace receipts::app
