#pragma once

#include <optional>
#include <variant>
#include <iostream>

#include <boost/json.hpp>
#include <boost/describe.hpp>
#include <App.h>

#include "accounts/shared/models.hpp"
#include "enumHelpers.hpp"

namespace accounts
{
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
            // std::cerr << "Caught error: \"{ \"code\": " << static_cast<int>(ptr->code) << ", \"message\":" << ptr->message << "}\"'\n";
        }

        return ptr;
    }

    template <class TValidationResult>
    bool abortIfValidationFailed(uWS::HttpResponse<false> *res, std::optional<TValidationResult> obj)
    {
        using namespace ::boost::json;

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
                        .errors = std::move(modelErrors),
                        .code = enumToString(EAppErrorCode::VALIDATION_ERROR),
                        .message = "Validation error"})))));

        return true;
    }

    void abort(uWS::HttpResponse<false> *res, AppError);

    bool abortIfUnauthorized(uWS::HttpResponse<false> *res, const std::optional<AuthUser> &authUser);

    template <class... Args>
    bool abortIfAppError(uWS::HttpResponse<false> *res, std::variant<Args...> *args)
    {
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