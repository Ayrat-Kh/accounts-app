#pragma once

// A fixed-size pool of PgConnections with one entry point:
//
//     auto result = co_await pool.query("SELECT ... WHERE user_id = $1", userId);
//
// Connections are opened lazily, reused, and dropped when unhealthy. When all
// of them are busy, callers wait (without blocking the thread) until one is
// returned.
//
// Not thread-safe by design: the whole server runs one io_context on one
// thread (see main.cpp), so no locks are needed.

#include "db/PgConnection.hpp"

#include <boost/asio/steady_timer.hpp>

#include <deque>
#include <memory>
#include <string>

namespace accounts::db {

class PgPool {
public:
    PgPool(std::string connectionString, std::size_t maxConnections);

    template <class... Args>
    boost::asio::awaitable<Result<PgResult>> query(std::string sql, const Args &...args) {
        // Arguments are converted here, before the coroutine starts, so no
        // reference to the caller's temporaries outlives this call.
        return execute(std::move(sql), {toPgParam(args)...});
    }

    // For INSERT/UPDATE/DELETE whose rows the caller doesn't need.
    template <class... Args>
    boost::asio::awaitable<Result<void>> command(std::string sql, const Args &...args) {
        return executeCommand(std::move(sql), {toPgParam(args)...});
    }

    boost::asio::awaitable<Result<PgResult>> execute(std::string sql, std::vector<PgParam> params);
    boost::asio::awaitable<Result<void>> executeCommand(std::string sql, std::vector<PgParam> params);

private:
    boost::asio::awaitable<Result<std::unique_ptr<PgConnection>>> acquire();
    void release(std::unique_ptr<PgConnection> connection);

    std::string connectionString_;
    std::size_t maxConnections_;
    std::size_t openConnections_ = 0;
    std::deque<std::unique_ptr<PgConnection>> idle_;
    // Each waiting coroutine sleeps on its own timer; release() cancels the
    // oldest one to wake it up (first come, first served).
    std::deque<std::shared_ptr<boost::asio::steady_timer>> waiters_;
};

} // namespace accounts::db
