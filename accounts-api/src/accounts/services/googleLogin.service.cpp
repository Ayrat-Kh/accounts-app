#include <boost/json.hpp>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/this_coro.hpp>

#include "googleLogin.service.hpp"
#include "sendHttpsRequest.hpp"
#include "accounts/utils/enumHelpers.hpp"

using namespace accounts;

boost::asio::awaitable<AccountsResult<GoogleTokenInfo>> GoogleLoginServiceImpl::getGoogleUser(std::string_view idToken) noexcept
{
    namespace beast = boost::beast;
    namespace http = beast::http;

    // Set up an HTTP POST request
    http::request<http::string_body> req;
    req.method(http::verb::post);
    req.target("/oauth2/v3/tokeninfo");                                      // Specify your API endpoint
    req.version(11);                                                         // HTTP/1.1
    req.set(http::field::content_type, "application/x-www-form-urlencoded"); // Content type
    req.body() = "id_token=" + std::string(idToken);                         // Your POST data
    req.prepare_payload();

    try
    {
        auto response = co_await sendHttpsRequest("www.googleapis.com", std::move(req));

        if (response.has_error())
        {
            co_return std::move(response.error());
        }

        // Parse the JSON response
        boost::json::value json_response = boost::json::parse(response.value().body());

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