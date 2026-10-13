#include "db/PgPool.hpp"

#include <boost/asio/as_tuple.hpp>
#include <boost/asio/this_coro.hpp>
#include <boost/asio/use_awaitable.hpp>

namespace accounts::db {

namespace asio = boost::asio;
using asio::awaitable;

PgPool::PgPool(std::string connectionString, std::size_t maxConnections)
    : connectionString_(std::move(connectionString)), maxConnections_(maxConnections) {}

awaitable<Result<PgResult>> PgPool::execute(std::string sql, std::vector<PgParam> params) {
    auto connection = co_await acquire();
    if (!connection) {
        co_return std::unexpected(connection.error());
    }
    auto result = co_await (*connection)->execute(std::move(sql), std::move(params));
    release(std::move(*connection));
    co_return result;
}

awaitable<Result<void>> PgPool::executeCommand(std::string sql, std::vector<PgParam> params) {
    auto result = co_await execute(std::move(sql), std::move(params));
    if (!result) {
        co_return std::unexpected(result.error());
    }
    co_return Result<void>{};
}

awaitable<Result<std::unique_ptr<PgConnection>>> PgPool::acquire() {
    for (;;) {
        if (!idle_.empty()) {
            auto connection = std::move(idle_.front());
            idle_.pop_front();
            if (connection->isHealthy()) {
                co_return connection;
            }
            --openConnections_; // dropped; try the next one or open a new one
            continue;
        }

        if (openConnections_ < maxConnections_) {
            ++openConnections_;
            auto connection = co_await PgConnection::connect(connectionString_);
            if (!connection) {
                --openConnections_;
            }
            co_return connection;
        }

        auto waiter = std::make_shared<asio::steady_timer>(co_await asio::this_coro::executor,
                                                           asio::steady_timer::time_point::max());
        waiters_.push_back(waiter);
        co_await waiter->async_wait(asio::as_tuple(asio::use_awaitable)); // "error" = woken up
    }
}

void PgPool::release(std::unique_ptr<PgConnection> connection) {
    if (connection->isHealthy()) {
        idle_.push_back(std::move(connection));
    } else {
        --openConnections_;
    }
    if (!waiters_.empty()) {
        waiters_.front()->cancel();
        waiters_.pop_front();
    }
}

} // namespace accounts::db
