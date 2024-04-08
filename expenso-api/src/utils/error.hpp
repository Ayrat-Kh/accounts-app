#pragma once

#include <optional>
#include <variant>
#include <iostream>

#include <boost/json.hpp>
#include <App.h>

#include "services/models.service.hpp"

namespace app::error
{
    enum class AppErrorCode
    {
        APP_ERROR,
        INVALID_INPUT,

        DB_NOT_FOUND = 50,
        DB_INSERT_ERROR,
        DB_QUERY_ERROR,

        THIRD_PARTY_REQUEST = 100
    };

    struct AppError
    {
        std::string message;

        AppErrorCode code;
    };

    template <class... Args>
    AppError *isError(std::variant<Args...> *args)
    {
        return std::get_if<AppError>(args);
    }

    template <class... Args>
    AppError *logIfError(std::variant<Args...> &args)
    {
        AppError *ptr = isError(args);

        if (ptr != nullptr)
        {
            std::cerr << "Caught error: \"{ \"code\": " << static_cast<int>(ptr->code) << ", \"message\":" << ptr->message << "}\"'\n";
        }

        return ptr;
    }

    bool abortIfValidationFailed(uWS::HttpResponse<false> *res, std::optional<boost::json::object>);
    void abort(uWS::HttpResponse<false> *res, boost::json::object);

    bool abortIfUnauthorized(uWS::HttpResponse<false> *res, const std::optional<app::services::AuthUser> &authUser);

    template <class... Args>
    bool abortIfAppError(uWS::HttpResponse<false> *res, std::variant<Args...> *args)
    {
        AppError *ptr = isError(args);

        if (ptr != nullptr)
        {
            boost::json::object error;
            error["code"] = static_cast<int>(ptr->code);
            error["message"] = std::move(ptr->message);

            res->writeHeader("Content-Type", "application/json")
                ->writeStatus("400")
                ->end(boost::json::serialize(error));

            return true;
        }

        return false;
    }
}