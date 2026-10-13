#include "support/TestHttpClient.hpp"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/this_coro.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/write.hpp>
#include <boost/json/parse.hpp>

#include <algorithm>
#include <cctype>

namespace accounts::testing {

namespace asio = boost::asio;
namespace beast = boost::beast;

boost::json::value TestResponse::json() const {
    return boost::json::parse(body);
}

asio::awaitable<TestResponse> sendRequest(std::uint16_t port, TestRequest request) {
    beast::tcp_stream stream(co_await asio::this_coro::executor);
    co_await stream.async_connect(asio::ip::tcp::endpoint(asio::ip::make_address("127.0.0.1"), port),
                                  asio::use_awaitable);

    beast::http::request<beast::http::string_body> raw{beast::http::string_to_verb(request.method), request.target,
                                                        11};
    raw.set(beast::http::field::host, "localhost");
    raw.set(beast::http::field::connection, "close");
    for (const auto &[name, value] : request.headers) {
        raw.set(name, value);
    }
    raw.body() = std::move(request.body);
    raw.prepare_payload();
    co_await beast::http::async_write(stream, raw, asio::use_awaitable);

    beast::flat_buffer buffer;
    beast::http::response_parser<beast::http::string_body> parser;
    parser.body_limit(64 * 1024 * 1024);
    co_await beast::http::async_read(stream, buffer, parser, asio::use_awaitable);
    auto response = parser.release();

    TestResponse result{static_cast<int>(response.result_int()), {}, std::move(response.body())};
    for (const auto &field : response) {
        std::string name(field.name_string());
        std::ranges::transform(name, name.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        result.headers[name] = std::string(field.value());
    }
    boost::system::error_code ignored;
    stream.socket().shutdown(asio::ip::tcp::socket::shutdown_both, ignored);
    co_return result;
}

} // namespace accounts::testing
