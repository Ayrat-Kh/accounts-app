#pragma once

#include <optional>
#include <variant>
#include <iostream>

#include <boost/json.hpp>
#include <boost/describe.hpp>
#include <App.h>

#include "accounts/shared/models.hpp"
#include "enumToString.hpp"

namespace accounts::error
{
    template <class... Args>
    shared::AppError *isError(std::variant<Args...> *args)
    {
        return std::get_if<shared::AppError>(args);
    }

    template <class... Args>
    shared::AppError *logIfError(std::variant<Args...> &args)
    {
        shared::AppError *ptr = isError(args);

        if (ptr != nullptr)
        {
            // std::cerr << "Caught error: \"{ \"code\": " << static_cast<int>(ptr->code) << ", \"message\":" << ptr->message << "}\"'\n";
        }

        return ptr;
    }

    template <class TValidationResult>
    bool abortIfValidationFailed(uWS::HttpResponse<false> *res, std::optional<TValidationResult> obj)
    {
        using namespace ::boost::json;
        using namespace ::accounts::shared;
        using namespace ::accounts::utils;

        if (!obj.has_value())
        {
            return false;
        }

        auto &&modelErrors = value_from(std::move(obj));

        res->writeHeader("Content-Type", "application/json")
            ->writeStatus("400")
            ->end(serialize(
                std::move(value_from(
                    std::move(ValidationAppError{
                        .code = enumToString(AppErrorCode::VALIDATION_ERROR),
                        .errors = std::move(modelErrors),
                        .message = "Validation error"})))));

        return true;
    }

    void abort(uWS::HttpResponse<false> *res, shared::AppError);

    bool abortIfUnauthorized(uWS::HttpResponse<false> *res, const std::optional<::accounts::shared::AuthUser> &authUser);

    template <class... Args>
    bool abortIfAppError(uWS::HttpResponse<false> *res, std::variant<Args...> *args)
    {
        using namespace ::accounts::shared;

        AppError *ptr = isError(args);

        if (ptr == nullptr)
        {
            return false;
        }

        boost::json::object error;
        error["code"] = ptr->code;
        error["message"] = std::move(ptr->message);

        res->writeHeader("Content-Type", "application/json")
            ->writeStatus("400")
            ->end(boost::json::serialize(error));

        return true;
    }
}