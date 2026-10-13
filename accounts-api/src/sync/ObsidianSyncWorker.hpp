#pragma once

// A background coroutine that periodically publishes receipts which are not
// yet in Obsidian (receipts.obsidian_synced_at IS NULL) and marks them done.
// Editing a receipt clears the mark, so the change is published again.

#include "receipts/ReceiptRepository.hpp"
#include "sync/ObsidianSink.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>

#include <chrono>

namespace accounts::sync {

class ObsidianSyncWorker {
public:
    ObsidianSyncWorker(boost::asio::io_context &ioContext, receipts::ReceiptRepository &repository,
                       ObsidianSink &sink, std::chrono::seconds interval);

    void start();
    void stop();

private:
    boost::asio::awaitable<void> runLoop();
    boost::asio::awaitable<void> publishPendingReceipts();

    boost::asio::io_context &ioContext_;
    receipts::ReceiptRepository &repository_;
    ObsidianSink &sink_;
    std::chrono::seconds interval_;
    boost::asio::steady_timer timer_;
    bool stopped_ = false;
};

} // namespace accounts::sync
