#include "error.hpp"

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
