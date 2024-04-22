#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "accounts.handlers.hpp"
#include "accounts/services/appDependencies.hpp"
#include "accounts/utils/error.hpp"
#include "accounts/utils/userId.hpp"
#include "accounts/utils/objectMapper.hpp"
#include "accounts/utils/readRequestJson.hpp"
#include "accounts/accounts/accounts.models.hpp"

void accounts::handleGetAccountsByUserId(uWS::HttpResponse<false> *res, uWS::HttpRequest *req)
{
    auto authToken = req->getHeader("authorization");
    auto &&authUser = AppDependencies::instance().jwtService->getAuthUser(authToken);
    if (abortIfUnauthorized(res, authUser))
    {
        return;
    }

    auto userId = req->getParameter("userId");

    if (userId != authUser.value().userId)
    {
        abort(
            res,
            std::move(
                AppError{
                    .code = enumToString(EAppErrorCode::VALIDATION_ERROR),
                    .message = "Only can fetch account information about yourself"}));

        return;
    }

    if (isMe(userId))
    {
        userId = authUser.value().userId;
    }

    auto &&accountsDb = AppDependencies::instance().accountsRepo->getAccountsByUserId(userId);

    if (abortIfAppError(res, &accountsDb))
    {
        return;
    }

    auto &&accountsDbCasted = std::get<std::vector<AccountDb>>(std::move(accountsDb));

    res
        ->writeHeader("Content-Type", "application/json")
        ->end(boost::json::serialize(std::move(boost::json::value_from(
            std::move(accountsDbCasted)))));
}

void accounts::handleAddAccount(uWS::HttpResponse<false> *_res, uWS::HttpRequest *_req)
{
    auto handler = [](uWS::HttpResponse<false> *res, uWS::HttpRequest *req, UpsertAccountDto parsedBody) mutable
    {
        auto authToken = req->getHeader("authorization");

        auto &&authUser = AppDependencies::instance().jwtService->getAuthUser(authToken);

        if (abortIfUnauthorized(res, authUser))
        {
            return;
        }

        UpsertAccountDb &&saveAccountDb = remapObject<UpsertAccountDb, UpsertAccountDto>(std::move(parsedBody));
        auto &&accountDb = AppDependencies::instance().accountsRepo->upsertAccount(
            "acc_" + boost::uuids::to_string(boost::uuids::random_generator()()),
            std::move(saveAccountDb));

        if (abortIfAppError(res, &accountDb))
        {
            return;
        }

        auto &&accountDbCasted = std::get<AccountDb>(std::move(accountDb));

        res
            ->writeHeader("Content-Type", "application/json")
            ->end(boost::json::serialize(std::move(boost::json::value_from(
                std::move(accountDbCasted)))));
    };

    RequestJsonBodyReader reader;
    reader.read<UpsertAccountDto>(_res, _req, handler);
}

void accounts::handleUpdateAccount(uWS::HttpResponse<false> *_res, uWS::HttpRequest *_req)
{
    auto handler = [](uWS::HttpResponse<false> *res, uWS::HttpRequest *req, UpsertAccountDto parsedBody) mutable
    {
        auto authToken = req->getHeader("authorization");
        auto accountId = req->getParameter("accountId");

        auto &&authUser = AppDependencies::instance().jwtService->getAuthUser(authToken);

        if (abortIfUnauthorized(res, authUser))
        {
            return;
        }

        if (parsedBody.userId != authUser.value().userId)
        {
            abort(
                res,
                std::move(
                    AppError{
                        .code = enumToString(EAppErrorCode::VALIDATION_ERROR),
                        .message = "Only can update information about your accounts"}));

            return;
        }

        UpsertAccountDb &&updateAccountDb = remapObject<UpsertAccountDb, UpsertAccountDto>(std::move(parsedBody));
        updateAccountDb.updatedAt = std::chrono::system_clock::now();

        auto &&accountDb = AppDependencies::instance().accountsRepo->upsertAccount(
            accountId, std::move(updateAccountDb));

        if (abortIfAppError(res, &accountDb))
        {
            return;
        }

        auto &&accountDbCasted = std::get<AccountDb>(std::move(accountDb));

        res
            ->writeHeader("Content-Type", "application/json")
            ->end(boost::json::serialize(std::move(boost::json::value_from(
                std::move(accountDbCasted)))));
    };

    RequestJsonBodyReader reader;
    reader.read<UpsertAccountDto>(_res, _req, handler);
}