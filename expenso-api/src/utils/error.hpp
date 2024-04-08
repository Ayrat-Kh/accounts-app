#pragma once

#include <optional>
#include <variant>
#include <iostream>

#include <boost/json.hpp>
#include <boost/describe.hpp>
#include <App.h>

#include "shared/models.hpp"
#include "services/models.service.hpp"
#include "enumToString.hpp"

namespace app::error
{
    template <class... Args>
    app::shared::AppError *isError(std::variant<Args...> *args)
    {
        return std::get_if<app::shared::AppError>(args);
    }

    template <class... Args>
    app::shared::AppError *logIfError(std::variant<Args...> &args)
    {
        app::shared::AppError *ptr = isError(args);

        if (ptr != nullptr)
        {
            // std::cerr << "Caught error: \"{ \"code\": " << static_cast<int>(ptr->code) << ", \"message\":" << ptr->message << "}\"'\n";
        }

        return ptr;
    }

    template <class TValidationResult>
    bool abortIfValidationFailed(uWS::HttpResponse<false> *res, std::optional<TValidationResult> obj)
    {
        using namespace boost::json;
        using namespace app::shared;
        using namespace app::utils;

        if (obj.has_value())
        {
            auto &&modelErrors = value_from(std::move(obj));

            res->writeHeader("Content-Type", "application/json")
                ->writeStatus("400")
                ->end(serialize(
                    std::move(value_from(
                        std::move(app::shared::ValidationAppError{
                            .code = app::utils::enumToString(AppErrorCode::VALIDATION_ERROR),
                            .errors = std::move(modelErrors),
                            .message = "Validation error"})))));

            return true;
        }

        return false;
    }

    void abort(uWS::HttpResponse<false> *res, app::shared::AppError);

    bool abortIfUnauthorized(uWS::HttpResponse<false> *res, const std::optional<app::services::AuthUser> &authUser);

    template <class... Args>
    bool abortIfAppError(uWS::HttpResponse<false> *res, std::variant<Args...> *args)
    {
        app::shared::AppError *ptr = isError(args);

        if (ptr != nullptr)
        {
            boost::json::object error;
            error["code"] = ptr->code;
            error["message"] = std::move(ptr->message);

            res->writeHeader("Content-Type", "application/json")
                ->writeStatus("400")
                ->end(boost::json::serialize(error));

            return true;
        }

        return false;
    }
}