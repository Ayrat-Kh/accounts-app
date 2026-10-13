#pragma once

// Shared helpers for the test suite.

#include "util/Error.hpp"

#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/io_context.hpp>

#include <exception>
#include <optional>
#include <string>

namespace accounts::testing {

// One io_context for the whole test run. uWebSockets binds its event loop to
// the first io_context it sees on a thread, so every test must share it.
boost::asio::io_context &testIoContext();

// Runs a coroutine to completion and returns its result:
//     auto receipt = runSync(repository.create(userId, draft));
template <class T>
T runSync(boost::asio::awaitable<T> task) {
    auto &ioContext = testIoContext();
    std::optional<T> result;
    std::exception_ptr failure;
    boost::asio::co_spawn(ioContext, std::move(task), [&](std::exception_ptr error, T value) {
        failure = error;
        if (!error) {
            result.emplace(std::move(value));
        }
    });
    ioContext.restart();
    while (!result && !failure) {
        ioContext.run_one();
    }
    // An io_context that ran out of work marks itself stopped; leave it
    // usable for the next test.
    ioContext.restart();
    if (failure) {
        std::rethrow_exception(failure);
    }
    return std::move(*result);
}

// A freshly generated Ed25519 private key in PEM form.
std::string generateEd25519PrivateKeyPem();

// A random UUID-ish suffix, so tests never collide on unique columns.
std::string uniqueSuffix();

} // namespace accounts::testing
