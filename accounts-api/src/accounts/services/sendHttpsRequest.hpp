#pragma once

#include <string_view>

#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/beast/http/write.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/this_coro.hpp>

#include "accounts/shared/models.hpp"
#include "accounts/utils/enumHelpers.hpp"

namespace accounts
{
    template <class TBody>
    boost::asio::awaitable<accounts::AccountsResult<boost::beast::http::response<TBody>>> sendHttpsRequest(
        std::string_view host,
        boost::beast::http::request<TBody> query) noexcept
    {
        namespace beast = boost::beast;
        namespace http = beast::http;
        namespace net = boost::asio;
        namespace ssl = net::ssl;

        using tcp_stream = beast::tcp_stream;
        using tcp = net::ip::tcp;

        query.set(http::field::host, host);
        query.prepare_payload();

        try
        {
            // net::io_context io_context;
            ssl::context ctx(ssl::context::sslv23_client);
            ctx.set_default_verify_paths();

            auto executor = co_await boost::asio::this_coro::executor;

            // Resolve the hostname
            tcp::resolver resolver(executor);
            auto const endpoints = resolver.resolve(host, "https");

            beast::ssl_stream<tcp_stream> sslStream(executor, ctx);

            // Establish a connection
            co_await sslStream.next_layer().async_connect(endpoints, boost::asio::use_awaitable);
            co_await sslStream.async_handshake(ssl::stream_base::client, boost::asio::use_awaitable);

            auto sent = co_await http::async_write(sslStream, query, boost::asio::use_awaitable);

            beast::flat_buffer buffer;
            http::response<http::string_body> res;
            co_await http::async_read(sslStream, buffer, res, boost::asio::use_awaitable);

            // Parse the JSON response
            co_return std::move(res);
        }
        catch (const std::exception &e)
        {
            co_return AppError{
                .code = enumToString(EAppErrorCode::THIRD_PARTY_REQUEST),
                .message = "get google id token info " + std::string(e.what())};
        }
    }

}
