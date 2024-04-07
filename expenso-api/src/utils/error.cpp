#include "error.hpp"

void app::error::abort(uWS::HttpResponse<false> *res, boost::json::object json)
{
    res->writeHeader("Content-Type", "application/json")
        ->writeStatus("400")
        ->end(boost::json::serialize(std::move(json)));
}

bool app::error::abortIfValidationFailed(uWS::HttpResponse<false> *res, std::optional<boost::json::object> obj)
{
    if (obj.has_value())
    {
        res->writeHeader("Content-Type", "application/json")
            ->writeStatus("400")
            ->end(boost::json::serialize(std::move(obj.value())));
        return true;
    }

    return false;
}

bool app::error::abortIfUnauthorized(uWS::HttpResponse<false> *res, const std::optional<app::services::AuthUser> &authUser)
{
    if (!authUser.has_value())
    {
        res->writeHeader("Content-Type", "application/json")
            ->writeStatus("401")
            ->end(boost::json::serialize(boost::json::object({{"message", "Unauthorized"}})));

        return true;
    }

    return false;
}
