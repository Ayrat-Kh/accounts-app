#include "app/SyncEngine.hpp"

#include "util/Files.hpp"

namespace receipts::app {

namespace {

// Errors that mean "stop and retry later" rather than "this one receipt is bad".
bool isTransient(const Error &error) {
    return error.code == ErrorCode::Network || error.code == ErrorCode::NotSignedIn;
}

} // namespace

Result<SyncReport> SyncEngine::sync() {
    SyncReport report;
    if (auto uploaded = pushUploads(report); !uploaded) {
        return std::unexpected(uploaded.error());
    }
    if (auto deleted = pushDeletes(report); !deleted) {
        return std::unexpected(deleted.error());
    }
    if (auto pulled = pull(report); !pulled) {
        return std::unexpected(pulled.error());
    }
    return report;
}

Result<void> SyncEngine::upload(Receipt receipt) {
    if (!receipt.serverId) {
        auto created = api_.createReceipt(receipt);
        if (!created) {
            return std::unexpected(created.error());
        }
        // Recorded right away: if the photo upload fails, a retry must not create a duplicate.
        receipt.serverId = created->serverId;
        if (auto marked = store_.markUploaded(receipt.localId, *receipt.serverId); !marked) {
            return marked;
        }
    }
    if (receipt.imagePath && !receipt.hasImageOnServer) {
        auto bytes = files::readAll(*receipt.imagePath);
        if (!bytes) {
            return std::unexpected(bytes.error());
        }
        if (auto uploaded = api_.uploadImage(*receipt.serverId, *bytes); !uploaded) {
            return uploaded;
        }
        if (auto marked = store_.markImageUploaded(receipt.localId); !marked) {
            return marked;
        }
    }
    return store_.markSynced(receipt.localId);
}

Result<void> SyncEngine::pushUploads(SyncReport &report) {
    auto pending = store_.listByState(SyncState::PendingUpload);
    if (!pending) {
        return std::unexpected(pending.error());
    }
    for (Receipt &receipt : *pending) {
        const std::string localId = receipt.localId;
        if (auto uploaded = upload(std::move(receipt)); !uploaded) {
            if (isTransient(uploaded.error())) {
                return uploaded;
            }
            logger_.log(LogLevel::Warning, "receipt " + localId + " not uploaded: " + uploaded.error().message);
            ++report.failed;
            continue;
        }
        ++report.uploaded;
    }
    return {};
}

Result<void> SyncEngine::pushDeletes(SyncReport &report) {
    auto pending = store_.listByState(SyncState::PendingDelete);
    if (!pending) {
        return std::unexpected(pending.error());
    }
    for (const Receipt &receipt : *pending) {
        if (auto deleted = api_.deleteReceipt(*receipt.serverId); !deleted) {
            if (isTransient(deleted.error())) {
                return deleted;
            }
            ++report.failed;
            continue;
        }
        if (auto removed = store_.removeLocal(receipt.localId); !removed) {
            return removed;
        }
        ++report.deleted;
    }
    return {};
}

Result<void> SyncEngine::pull(SyncReport &report) {
    auto serverReceipts = api_.listAllReceipts();
    if (!serverReceipts) {
        return std::unexpected(serverReceipts.error());
    }
    auto added = store_.mergeFromServer(*serverReceipts);
    if (!added) {
        return std::unexpected(added.error());
    }
    report.downloaded = *added;
    return {};
}

} // namespace receipts::app
