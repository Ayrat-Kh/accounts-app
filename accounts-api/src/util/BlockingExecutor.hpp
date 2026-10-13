#pragma once

// Runs a blocking call (the AWS SDK has no coroutine API) on a small thread
// pool and resumes the calling coroutine on its own executor afterwards:
//
//     auto bytes = co_await blocking.run([&] { return s3.GetObject(request); });
//
// The event-loop thread is free to serve other requests in the meantime, and
// because the coroutine always hops back home, code after the co_await never
// runs on a pool thread, so it can use loop-only state such as the PgPool safely.

#include <boost/asio/awaitable.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/this_coro.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/use_awaitable.hpp>

#include <exception>
#include <optional>
#include <type_traits>

namespace accounts {

class BlockingExecutor {
public:
    explicit BlockingExecutor(std::size_t threadCount) : pool_(threadCount) {}
    ~BlockingExecutor() { pool_.join(); }

    BlockingExecutor(const BlockingExecutor &) = delete;
    BlockingExecutor &operator=(const BlockingExecutor &) = delete;

    template <class Function>
    boost::asio::awaitable<std::invoke_result_t<Function>> run(Function function) {
        using Value = std::invoke_result_t<Function>;
        auto home = co_await boost::asio::this_coro::executor;

        co_await boost::asio::post(pool_.get_executor(), boost::asio::use_awaitable);
        // Now on a pool thread. Capture the outcome instead of letting an
        // exception unwind the coroutine here, on the wrong thread.
        std::optional<Value> value;
        std::exception_ptr error;
        try {
            value.emplace(function());
        } catch (...) {
            error = std::current_exception();
        }

        co_await boost::asio::post(home, boost::asio::use_awaitable);
        // Back on the caller's executor.
        if (error) {
            std::rethrow_exception(error);
        }
        co_return std::move(*value);
    }

private:
    boost::asio::thread_pool pool_;
};

} // namespace accounts
