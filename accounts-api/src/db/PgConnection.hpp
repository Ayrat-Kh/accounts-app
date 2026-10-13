#pragma once

// One Postgres connection, driven without ever blocking the event loop.
//
// libpq is a blocking library by default. In non-blocking mode it instead
// says "I need the socket to become readable/writable", and we wait for that
// with Asio (`co_await socket.async_wait(...)`). While one coroutine waits
// for Postgres, the same thread serves other HTTP requests. This is what the
// old Mongo driver could not do.
//
// A connection runs one statement at a time; PgPool hands connections out.

#include "db/PgParam.hpp"
#include "db/PgResult.hpp"
#include "util/Error.hpp"

#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>

#include <libpq-fe.h>

#include <memory>
#include <string>
#include <vector>

namespace accounts::db {

class PgConnection {
public:
    static boost::asio::awaitable<Result<std::unique_ptr<PgConnection>>> connect(std::string connectionString);

    ~PgConnection();
    PgConnection(const PgConnection &) = delete;
    PgConnection &operator=(const PgConnection &) = delete;

    // One parameterised statement ($1, $2, ...). Never build SQL by string concatenation.
    boost::asio::awaitable<Result<PgResult>> execute(std::string sql, std::vector<PgParam> params);

    // Several ';'-separated statements without parameters (migrations only).
    boost::asio::awaitable<Result<void>> executeScript(std::string sql);

    // False once the server closed the connection or a statement timed out
    // mid-flight; the pool then drops it instead of reusing it.
    bool isHealthy() const;

private:
    enum class Direction { Read, Write };

    PgConnection(const boost::asio::any_io_executor &executor, PGconn *connection);

    boost::asio::awaitable<Result<void>> waitForSocket(Direction direction);
    boost::asio::awaitable<Result<void>> flushOutgoingData();
    // Reads every result of the statement that was just sent and returns the
    // first one; any failing result turns into an error.
    boost::asio::awaitable<Result<PgResult>> collectResults();

    AppError lastError(std::string_view context) const;

    PGconn *connection_;
    boost::asio::posix::stream_descriptor socket_;
    bool broken_ = false;
};

} // namespace accounts::db
