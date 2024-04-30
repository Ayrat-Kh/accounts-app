#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/beast/http/write.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/json.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/this_coro.hpp>

#include "googleLogin.service.hpp"
#include "accounts/utils/enumHelpers.hpp"

using namespace accounts;

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;

using tcp_stream = beast::tcp_stream;

boost::asio::awaitable<std::variant<GoogleTokenInfo, AppError>> GoogleLoginServiceImpl::getGoogleUser(std::string_view idToken) noexcept
{
    using tcp = net::ip::tcp;

    // Set up an HTTP POST request
    http::request<http::string_body>
        req;
    req.method(http::verb::post);
    req.target("/oauth2/v3/tokeninfo");                                      // Specify your API endpoint
    req.version(11);                                                         // HTTP/1.1
    req.set(http::field::host, "www.googleapis.com");                        // Set the host
    req.set(http::field::content_type, "application/x-www-form-urlencoded"); // Content type
    req.body() = "id_token=" + std::string(idToken);                         // Your POST data
    req.prepare_payload();

    try
    {
        // net::io_context io_context;
        ssl::context ctx(ssl::context::sslv23_client);
        ctx.set_default_verify_paths();

        // Resolve the hostname
        tcp::resolver resolver(co_await boost::asio::this_coro::executor);
        auto const endpoints = resolver.resolve("www.googleapis.com", "https");

        beast::ssl_stream<tcp_stream> sslStream(co_await boost::asio::this_coro::executor, ctx);

        // Establish a connection
        co_await sslStream.next_layer().async_connect(endpoints, boost::asio::use_awaitable);
        co_await sslStream.async_handshake(ssl::stream_base::client, boost::asio::use_awaitable);

        auto sent = co_await http::async_write(sslStream, req, boost::asio::use_awaitable);

        beast::flat_buffer buffer;
        http::response<http::string_body> res;
        co_await http::async_read(sslStream, buffer, res, boost::asio::use_awaitable);

        // Parse the JSON response
        boost::json::value json_response = boost::json::parse(res.body());

        if (!json_response.is_object())
        {
            co_return AppError{
                .code = enumToString(EAppErrorCode::THIRD_PARTY_REQUEST),
                .message = "get google id token info response is not json object"};
        }

        if (json_response.as_object().contains("error_description"))
        {
            co_return AppError{
                .code = enumToString(EAppErrorCode::THIRD_PARTY_REQUEST),
                .message = "get google id token info error_description: " + std::string(json_response.as_object()["error_description"].as_string())};
        }

        auto result = boost::json::try_value_to<GoogleTokenInfo>(json_response);

        if (result.has_error())
        {
            co_return AppError{
                .code = enumToString(EAppErrorCode::THIRD_PARTY_REQUEST),
                .message = std::move(result.error().what())};
        }

        co_return std::move(result.value());
    }
    catch (const std::exception &e)
    {
        co_return AppError{
            .code = enumToString(EAppErrorCode::THIRD_PARTY_REQUEST),
            .message = "get google id token info " + std::string(e.what())};
    }
}