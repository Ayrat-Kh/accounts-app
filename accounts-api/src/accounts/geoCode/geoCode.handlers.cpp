#include <charconv>
#include <iostream>

#include <boost/asio.hpp>

#include "geoCode.handlers.hpp"
#include "accounts/services/appDependencies.hpp"
#include "accounts/utils/error.hpp"
#include "accounts/utils/userId.hpp"

void accounts::handleDecodeGeoApi(uWS::HttpResponse<false> *_res, uWS::HttpRequest *req)
{
    auto authToken = req->getHeader("authorization");
    auto &&authUser = AppDependencies::instance().jwtService->getAuthUser(authToken);
    if (abortIfUnauthorized(_res, authUser))
    {
        return;
    }

    auto latParam = req->getQuery("lat");
    auto lonParam = req->getQuery("lon");

    if (latParam == "" || lonParam == "")
    {
        abort(_res, accounts::AppError{
                        .code = enumToString(EAppErrorCode::INVALID_INPUT),
                        .message = "lat and lon required params"});
    }

    char *endp = nullptr;
    float lat = std::strtof(latParam.data(), &endp);

    float lon = std::strtof(lonParam.data(), &endp);

    boost::asio::co_spawn(
        AppDependencies::instance().ioContext,
        [lat, lon](uWS::HttpResponse<false> *res) mutable -> boost::asio::awaitable<void>
        {
            auto &&result = co_await AppDependencies::instance().geoLocReverseService->getGeoReverse(
                {.lat = lat,
                 .lon = lon});

            if (abortRequestIfAppError(res, &result))
            {
                co_return;
            }

            res
                ->writeHeader("Content-Type", "application/json")
                ->end(boost::json::serialize(std::move(boost::json::value_from(
                    std::move(result.value())))));
        }(_res->onAborted(
            [_res]()
            { 
            std::cerr << "aborted request reverse geocode";
            _res->end("Aborted request"); })),
        boost::asio::detached);
}
