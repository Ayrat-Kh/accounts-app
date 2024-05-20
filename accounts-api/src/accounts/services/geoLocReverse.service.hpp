#pragma once

#include <string>

#include <boost/asio/awaitable.hpp>
#include <boost/describe.hpp>

#include "accounts/shared/models.hpp"

namespace accounts
{
    struct LocationRequestParams
    {
        float lat; // display_name
        float lon; // address.locality
    };

    struct GeocodeReverseResult
    {
        std::string name;         // display_name
        std::string locality;     // address.locality
        std::string municipality; // address.municipality
        std::string county;       // address.county
        std::string state;        // address.state
        std::string region;       // address.region
        std::string country;      // address.country
        std::string countryCode;  // address.country_code
    };
    BOOST_DESCRIBE_STRUCT(GeocodeReverseResult, (), (name, locality, municipality, county, state, region, country, countryCode))

    struct IGeoLocReverseService
    {
    public:
        virtual boost::asio::awaitable<AccountsResult<GeocodeReverseResult>> getGeoReverse(LocationRequestParams params) = 0;
    };

    class GeoLocReverseServiceImpl : public IGeoLocReverseService
    {
        std::string _apiKey;

    public:
        GeoLocReverseServiceImpl(std::string apiKey);

        virtual boost::asio::awaitable<AccountsResult<GeocodeReverseResult>> getGeoReverse(LocationRequestParams params) noexcept override;
    };
}
