#include "googleLogin.service.hpp"

#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio.hpp>
#include <boost/json.hpp>

#include "accounts/utils/enumHelpers.hpp"

using namespace accounts;

std::variant<GoogleTokenInfo, AppError> GoogleLoginServiceImpl::getGoogleUser(std::string_view idToken) noexcept
{
    namespace beast = boost::beast;
    namespace http = beast::http;
    namespace net = boost::asio;
    namespace ssl = net::ssl;
    using tcp = net::ip::tcp;

    boost::json::value json_response;

    try
    {
        net::io_context io_context;
        ssl::context ctx(ssl::context::sslv23_client);
        ctx.set_default_verify_paths();

        // Resolve the hostname
        tcp::resolver resolver(io_context);
        beast::ssl_stream<tcp::socket> stream(io_context, ctx);
        auto const endpoints = resolver.resolve("www.googleapis.com", "https");
        // Establish a connection
        net::connect(stream.next_layer(), endpoints);
        stream.handshake(ssl::stream_base::client);

        // Set up an HTTP POST request
        http::request<http::string_body> req;
        req.method(http::verb::post);
        req.target("/oauth2/v3/tokeninfo");                                      // Specify your API endpoint
        req.version(11);                                                         // HTTP/1.1
        req.set(http::field::host, "www.googleapis.com");                        // Set the host
        req.set(http::field::content_type, "application/x-www-form-urlencoded"); // Content type
        req.body() = "id_token=" + std::string(idToken);                         // Your POST data
        req.prepare_payload();

        // Send the request
        http::write(stream, req);

        beast::flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(stream, buffer, res);

        // Parse the JSON response
        json_response = boost::json::parse(res.body());

        if (!json_response.is_object())
        {
            return AppError{
                .code = enumToString(EAppErrorCode::THIRD_PARTY_REQUEST),
                .message = "get google id token info response is not json object"};
        }

        if (json_response.as_object().contains("error_description"))
        {
            return AppError{
                .code = enumToString(EAppErrorCode::THIRD_PARTY_REQUEST),
                .message = "get google id token info error_description: " + std::string(json_response.as_object()["error_description"].as_string())};
        }

        auto result = boost::json::try_value_to<GoogleTokenInfo>(json_response);

        if (result.has_error())
        {
            return AppError{
                .code = enumToString(EAppErrorCode::THIRD_PARTY_REQUEST),
                .message = std::move(result.error().what())};
        }

        return std::move(result.value());
    }
    catch (const std::exception &e)
    {
        return AppError{
            .code = enumToString(EAppErrorCode::THIRD_PARTY_REQUEST),
            .message = "get google id token info " + std::string(e.what())};
    }
}