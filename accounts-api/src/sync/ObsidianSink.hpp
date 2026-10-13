#pragma once

// Where receipts go to appear in the Obsidian vault. There is no
// implementation yet: obsync (../obsidian) has no write API for services.
// docs/obsidian-sync.md describes the follow-up. Until then, main.cpp starts
// no ObsidianSyncWorker.

#include "receipts/Receipt.hpp"

#include <boost/asio/awaitable.hpp>

namespace accounts::sync {

class ObsidianSink {
public:
    virtual ~ObsidianSink() = default;
    // Must be idempotent: after a crash the same receipt may be published twice.
    virtual boost::asio::awaitable<Result<void>> publish(const receipts::Receipt &receipt) = 0;
};

} // namespace accounts::sync
