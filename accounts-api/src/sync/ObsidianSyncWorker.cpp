#include "sync/ObsidianSyncWorker.hpp"

#include <boost/asio/as_tuple.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <spdlog/spdlog.h>

namespace accounts::sync {

namespace asio = boost::asio;
using asio::awaitable;

namespace {
constexpr int batchSize = 100;
}

ObsidianSyncWorker::ObsidianSyncWorker(asio::io_context &ioContext, receipts::ReceiptRepository &repository,
                                       ObsidianSink &sink, std::chrono::seconds interval)
    : ioContext_(ioContext), repository_(repository), sink_(sink), interval_(interval), timer_(ioContext) {}

void ObsidianSyncWorker::start() {
    asio::co_spawn(ioContext_, runLoop(), asio::detached);
}

void ObsidianSyncWorker::stop() {
    stopped_ = true;
    timer_.cancel();
}

awaitable<void> ObsidianSyncWorker::runLoop() {
    while (!stopped_) {
        co_await publishPendingReceipts();
        timer_.expires_after(interval_);
        co_await timer_.async_wait(asio::as_tuple(asio::use_awaitable)); // cancelled by stop()
    }
}

awaitable<void> ObsidianSyncWorker::publishPendingReceipts() {
    auto pending = co_await repository_.listNotYetInObsidian(batchSize);
    if (!pending) {
        spdlog::warn("obsidian sync: cannot list receipts: {}", pending.error().message);
        co_return;
    }
    for (const receipts::Receipt &receipt : *pending) {
        if (auto published = co_await sink_.publish(receipt); !published) {
            // Leave it unmarked; the next round retries.
            spdlog::warn("obsidian sync: receipt {} failed: {}", receipt.id, published.error().message);
            continue;
        }
        if (auto marked = co_await repository_.markInObsidian(receipt.id); !marked) {
            spdlog::warn("obsidian sync: cannot mark receipt {}: {}", receipt.id, marked.error().message);
        }
    }
}

} // namespace accounts::sync
