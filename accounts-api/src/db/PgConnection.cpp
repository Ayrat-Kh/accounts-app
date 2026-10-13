#include "db/PgConnection.hpp"

#include <boost/asio/as_tuple.hpp>
#include <boost/asio/cancel_after.hpp>
#include <boost/asio/this_coro.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <spdlog/spdlog.h>

#include <chrono>

namespace accounts::db {

namespace asio = boost::asio;
using asio::awaitable;
using asio::posix::descriptor_base;

namespace {

// An upper bound for any single wait on the socket. Postgres itself enforces
// statement_timeout; this only protects us from a silently dead peer.
constexpr auto socketWaitTimeout = std::chrono::seconds(30);

descriptor_base::wait_type toWaitType(bool forReading) {
    return forReading ? descriptor_base::wait_read : descriptor_base::wait_write;
}

// Waits on a raw libpq socket that we do not own (libpq closes it itself).
awaitable<boost::system::error_code> waitOnSocket(int fd, bool forReading) {
    asio::posix::stream_descriptor socket(co_await asio::this_coro::executor, fd);
    auto [error] = co_await socket.async_wait(
        toWaitType(forReading), asio::cancel_after(socketWaitTimeout, asio::as_tuple(asio::use_awaitable)));
    socket.release();  // hand the fd back without closing it
    co_return error;
}

} // namespace

awaitable<Result<std::unique_ptr<PgConnection>>> PgConnection::connect(std::string connectionString) {
    PGconn *raw = PQconnectStart(connectionString.c_str());
    if (raw == nullptr) {
        co_return fail(ErrorCode::UpstreamFailure, "postgres: out of memory");
    }
    std::unique_ptr<PGconn, decltype(&PQfinish)> connection(raw, &PQfinish);
    if (PQstatus(raw) == CONNECTION_BAD) {
        co_return fail(ErrorCode::UpstreamFailure, std::string("postgres: ") + PQerrorMessage(raw));
    }

    // The documented PQconnectPoll loop. The socket may change between
    // iterations (libpq tries each host in turn), so it is looked up every time.
    PostgresPollingStatusType status = PGRES_POLLING_WRITING;
    while (status != PGRES_POLLING_OK) {
        if (status == PGRES_POLLING_FAILED) {
            co_return fail(ErrorCode::UpstreamFailure, std::string("postgres connect: ") + PQerrorMessage(raw));
        }
        if (const auto error = co_await waitOnSocket(PQsocket(raw), status == PGRES_POLLING_READING)) {
            co_return fail(ErrorCode::UpstreamFailure, "postgres connect: " + error.message());
        }
        status = PQconnectPoll(raw);
    }

    // Server notices ("relation already exists, skipping") go to our log at
    // debug level instead of libpq's default, a raw write to stderr.
    PQsetNoticeProcessor(
        raw, [](void *, const char *message) { spdlog::debug("postgres: {}", message); }, nullptr);

    if (PQsetnonblocking(raw, 1) != 0) {
        co_return fail(ErrorCode::UpstreamFailure, std::string("postgres: ") + PQerrorMessage(raw));
    }
    auto executor = co_await asio::this_coro::executor;
    co_return std::unique_ptr<PgConnection>(new PgConnection(executor, connection.release()));
}

PgConnection::PgConnection(asio::any_io_executor executor, PGconn *connection)
    : connection_(connection), socket_(std::move(executor), PQsocket(connection)) {}

PgConnection::~PgConnection() {
    socket_.release();  // libpq owns the file descriptor...
    PQfinish(connection_);  // ...and closes it here.
}

bool PgConnection::isHealthy() const {
    return !broken_ && PQstatus(connection_) == CONNECTION_OK;
}

AppError PgConnection::lastError(std::string_view context) const {
    return {ErrorCode::UpstreamFailure, "postgres " + std::string(context) + ": " + PQerrorMessage(connection_)};
}

awaitable<Result<void>> PgConnection::waitForSocket(Direction direction) {
    auto [error] = co_await socket_.async_wait(
        toWaitType(direction == Direction::Read),
        asio::cancel_after(socketWaitTimeout, asio::as_tuple(asio::use_awaitable)));
    if (error) {
        // The statement is still in flight; the connection is in an unknown state.
        broken_ = true;
        co_return fail(ErrorCode::UpstreamFailure, "postgres socket: " + error.message());
    }
    co_return Result<void>{};
}

awaitable<Result<void>> PgConnection::flushOutgoingData() {
    // In non-blocking mode a large query may not fit in the socket buffer at
    // once; PQflush returns 1 while data is still queued.
    for (;;) {
        const int flushStatus = PQflush(connection_);
        if (flushStatus == 0) {
            co_return Result<void>{};
        }
        if (flushStatus < 0) {
            broken_ = true;
            co_return std::unexpected(lastError("flush"));
        }
        if (auto waited = co_await waitForSocket(Direction::Write); !waited) {
            co_return std::unexpected(waited.error());
        }
    }
}

awaitable<Result<PgResult>> PgConnection::collectResults() {
    std::optional<PgResult> first;
    std::optional<AppError> failure;

    for (;;) {
        // Wait until libpq has a complete result buffered...
        while (PQisBusy(connection_) == 1) {
            if (auto waited = co_await waitForSocket(Direction::Read); !waited) {
                co_return std::unexpected(waited.error());
            }
            if (PQconsumeInput(connection_) == 0) {
                broken_ = true;
                co_return std::unexpected(lastError("read"));
            }
        }
        // ...then take it. A null result means the statement is complete.
        PGresult *raw = PQgetResult(connection_);
        if (raw == nullptr) {
            break;
        }
        PgResult result(raw);
        const ExecStatusType status = PQresultStatus(raw);
        if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK && !failure) {
            failure = AppError{ErrorCode::UpstreamFailure, std::string("postgres: ") + PQresultErrorMessage(raw)};
        }
        if (!first) {
            first.emplace(std::move(result));
        }
    }

    if (failure) {
        co_return std::unexpected(*failure);
    }
    if (!first) {
        co_return fail(ErrorCode::Internal, "postgres: statement returned no result");
    }
    co_return std::move(*first);
}

awaitable<Result<PgResult>> PgConnection::execute(std::string sql, std::vector<PgParam> params) {
    std::vector<const char *> values;
    values.reserve(params.size());
    for (const PgParam &param : params) {
        values.push_back(param ? param->c_str() : nullptr);
    }

    const int sent = PQsendQueryParams(connection_, sql.c_str(), static_cast<int>(values.size()),
                                       /*paramTypes=*/nullptr, values.data(), /*paramLengths=*/nullptr,
                                       /*paramFormats=*/nullptr, /*resultFormat text=*/0);
    if (sent == 0) {
        co_return std::unexpected(lastError("send"));
    }
    if (auto flushed = co_await flushOutgoingData(); !flushed) {
        co_return std::unexpected(flushed.error());
    }
    co_return co_await collectResults();
}

awaitable<Result<void>> PgConnection::executeScript(std::string sql) {
    if (PQsendQuery(connection_, sql.c_str()) == 0) {
        co_return std::unexpected(lastError("send"));
    }
    if (auto flushed = co_await flushOutgoingData(); !flushed) {
        co_return std::unexpected(flushed.error());
    }
    auto result = co_await collectResults();
    if (!result) {
        co_return std::unexpected(result.error());
    }
    co_return Result<void>{};
}

} // namespace accounts::db
