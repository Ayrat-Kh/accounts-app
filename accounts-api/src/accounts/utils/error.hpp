#pragma once

#include <optional>
#include <iostream>

#include <boost/json.hpp>
#include <boost/describe.hpp>
#include <App.h>

#include "accounts/shared/models.hpp"
#include "enumHelpers.hpp"

namespace accounts
{
    template <class TValidationResult>
    bool abortIfValidationFailed(uWS::HttpResponse<false> *res, std::optional<TValidationResult> obj)
    {
        using namespace ::boost::json;

        if (!obj.has_value())
        {
            return false;
        }

        auto &&modelErrors = value_from(std::move(obj));

        res
            ->writeStatus("400")
            ->writeHeader("Content-Type", "application/json")
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

    template <class TResult>
    bool abortRequestIfAppError(uWS::HttpResponse<false> *res, AccountsResult<TResult> *args)
    {
        if (!args->has_error())
        {
            return false;
        }

        auto v = boost::json::serialize(std::move(boost::json::value_from(
            std::move(args->value()))));

        std::cerr << v;

        res->writeHeader("Content-Type", "application/json")
            ->writeStatus("400")
            ->end(v);

        return true;
    }
}