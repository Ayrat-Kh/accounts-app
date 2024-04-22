#include "error.hpp"

void accounts::abort(uWS::HttpResponse<false> *res, AppError json)
{
    res
        ->writeStatus("400")
        ->writeHeader("Content-Type", "application/json")
        ->end(boost::json::serialize(std::move(boost::json::value_from(
            std::move(json)))));
}

bool accounts::abortIfUnauthorized(uWS::HttpResponse<false> *res, const std::optional<AuthUser> &authUser)
{
    if (!authUser.has_value())
    {
        res
            ->writeStatus("401")
            ->writeHeader("Content-Type", "application/json")
            ->end(boost::json::serialize(boost::json::object({{"message", "Unauthorized"}})));

        return true;
    }

    return false;
}
