#include "error.hpp"

void accounts::abort(uWS::HttpResponse<false> *res, AppError json)
{
    res->writeHeader("Content-Type", "application/json")
        ->writeStatus("400")
        ->end(boost::json::serialize(std::move(boost::json::value_from(
            std::move(json)))));
}

bool accounts::abortIfUnauthorized(uWS::HttpResponse<false> *res, const std::optional<AuthUser> &authUser)
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
