#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/this_coro.hpp>
#include <boost/json.hpp>

#include "geoLocReverse.service.hpp"
#include "sendHttpsRequest.hpp"
#include "accounts/utils/enumHelpers.hpp"

using namespace accounts;

GeoLocReverseServiceImpl::GeoLocReverseServiceImpl(
    std::string apiKey)
    : _apiKey(std::move(apiKey))
{
}

boost::asio::awaitable<AccountsResult<GeocodeReverseResult>> GeoLocReverseServiceImpl::getGeoReverse(LocationRequestParams params) noexcept
{
    namespace beast = boost::beast;
    namespace http = beast::http;

    // Set up an HTTP POST request
    http::request<http::string_body> req;
    req.method(http::verb::post);
    req.target("/reverse");                                                                                         // Specify your API endpoint
    req.version(11);                                                                                                // HTTP/1.1
    req.set(http::field::content_type, "application/x-www-form-urlencoded");                                        // Content type
    req.body() = "lat=" + std::to_string(params.lat) + "&lon=" + std::to_string(params.lon) + "api_key=" + _apiKey; // Your POST data

    try
    {

        auto response = co_await sendHttpsRequest("geocode.maps.co", std::move(req));

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
                .message = "get geo reverse response is not json object"};
        }

        boost::json::object json_response_object = json_response.get_object();

        GeocodeReverseResult result;

        if (json_response_object["address"].is_object())
        {
            boost::json::object address_response_object = json_response_object["address"].as_object();

            if (address_response_object.find("country") != address_response_object.end() &&
                address_response_object["country"].is_string())
            {
                result.country = address_response_object["country"].as_string();
            }

            if (address_response_object.find("country_code") != address_response_object.end() &&
                address_response_object["country_code"].is_string())
            {
                result.countryCode = address_response_object["country_code"].as_string();
            }

            if (address_response_object.find("county") != address_response_object.end() &&
                address_response_object["county"].is_string())
            {
                result.county = address_response_object["county"].as_string();
            }

            if (address_response_object.find("locality") != address_response_object.end() &&
                address_response_object["locality"].is_string())
            {
                result.locality = address_response_object["locality"].as_string();
            }

            if (address_response_object.find("municipality") != address_response_object.end() &&
                address_response_object["municipality"].is_string())
            {
                result.municipality = address_response_object["municipality"].as_string();
            }

            if (address_response_object.find("region") != address_response_object.end() &&
                address_response_object["region"].is_string())
            {
                result.region = address_response_object["region"].as_string();
            }

            if (address_response_object.find("state") != address_response_object.end() &&
                address_response_object["state"].is_string())
            {
                result.state = address_response_object["state"].as_string();
            }
        }
        result.name = json_response_object.at("display_name").is_string() ? std::string(json_response_object.at("display_name").as_string()) : "";

        co_return std::move(result);
    }
    catch (const std::exception &e)
    {
        co_return AppError{
            .code = enumToString(EAppErrorCode::THIRD_PARTY_REQUEST),
            .message = "get google id token info " + std::string(e.what())};
    }
}