#include "net/HttpsClient.hpp"

#include <boost/asio/as_tuple.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/host_name_verification.hpp>
#include <boost/asio/this_coro.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/http/empty_body.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/write.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>

#include <chrono>

namespace accounts::net {

namespace asio = boost::asio;
namespace beast = boost::beast;
using asio::awaitable;

namespace {

constexpr auto requestTimeout = std::chrono::seconds(10);
constexpr auto useAwaitableNoThrow = asio::as_tuple(asio::use_awaitable);

AppError networkError(std::string_view step, const boost::system::error_code &error) {
    return {ErrorCode::UpstreamFailure, "https " + std::string(step) + ": " + error.message()};
}

} // namespace

HttpsClient::HttpsClient() : tlsContext_(asio::ssl::context::tls_client) {
    tlsContext_.set_default_verify_paths();
    tlsContext_.set_verify_mode(asio::ssl::verify_peer);
}

awaitable<Result<HttpsResponse>> HttpsClient::get(std::string host, std::string target) {
    auto executor = co_await asio::this_coro::executor;

    asio::ip::tcp::resolver resolver(executor);
    auto [resolveError, endpoints] = co_await resolver.async_resolve(host, "443", useAwaitableNoThrow);
    if (resolveError) {
        co_return std::unexpected(networkError("resolve", resolveError));
    }

    beast::ssl_stream<beast::tcp_stream> stream(executor, tlsContext_);
    // SNI (so the server picks the right certificate) and host name
    // verification (so the certificate must actually be for `host`).
    if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
        co_return fail(ErrorCode::Internal, "https: cannot set SNI host name");
    }
    stream.set_verify_callback(asio::ssl::host_name_verification(host));

    beast::get_lowest_layer(stream).expires_after(requestTimeout);
    if (auto [error, endpoint] = co_await beast::get_lowest_layer(stream).async_connect(endpoints, useAwaitableNoThrow);
        error) {
        co_return std::unexpected(networkError("connect", error));
    }
    if (auto [error] = co_await stream.async_handshake(asio::ssl::stream_base::client, useAwaitableNoThrow); error) {
        co_return std::unexpected(networkError("tls handshake", error));
    }

    beast::http::request<beast::http::empty_body> request{beast::http::verb::get, target, 11};
    request.set(beast::http::field::host, host);
    request.set(beast::http::field::user_agent, "accounts-api");
    if (auto [error, bytes] = co_await beast::http::async_write(stream, request, useAwaitableNoThrow); error) {
        co_return std::unexpected(networkError("write", error));
    }

    beast::flat_buffer buffer;
    beast::http::response<beast::http::string_body> response;
    if (auto [error, bytes] = co_await beast::http::async_read(stream, buffer, response, useAwaitableNoThrow); error) {
        co_return std::unexpected(networkError("read", error));
    }

    // A failed TLS close_notify is common and harmless once we have the response.
    co_await stream.async_shutdown(useAwaitableNoThrow);
    co_return HttpsResponse{static_cast<int>(response.result_int()), std::move(response.body())};
}

} // namespace accounts::net
