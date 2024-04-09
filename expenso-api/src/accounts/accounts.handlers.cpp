#include "accounts.handlers.hpp"
#include "services/appDependencies.hpp"
#include "utils/error.hpp"
#include "utils/readRequestJson.hpp"
#include "accounts/accounts.models.hpp"

void app::accounts::handleGetAccountsByUserId(uWS::HttpResponse<false> *res, uWS::HttpRequest *req)
{
    auto authToken = req->getHeader("authorization");
    auto &&authUser = app::services::AppDependencies::instance().jwtService->getAuthUser(authToken);
    if (app::error::abortIfUnauthorized(res, authUser))
    {
        return;
    }

    auto userId = req->getParameter("userId");
    auto &&accountsDb = app::services::AppDependencies::instance().accountsRepo->getAccountsByUserId(userId);

    if (app::error::abortIfAppError(res, &accountsDb))
    {
        return;
    }

    auto &&accountsDbCasted = std::get<std::vector<app::accounts::AccountDb>>(std::move(accountsDb));

    res
        ->writeHeader("Content-Type", "application/json")
        ->end(boost::json::serialize(std::move(boost::json::value_from(
            std::move(accountsDbCasted)))));
}

void app::accounts::handleAddAccount(uWS::HttpResponse<false> *_res, uWS::HttpRequest *_req)
{
    auto handler = [](uWS::HttpResponse<false> *res, uWS::HttpRequest *req, app::accounts::AccountDb parsedBody) mutable
    {
        auto authToken = req->getHeader("authorization");

        auto &&authUser = app::services::AppDependencies::instance().jwtService->getAuthUser(authToken);

        if (app::error::abortIfUnauthorized(res, authUser))
        {
            return;
        }

        auto &&accountDb = app::services::AppDependencies::instance().accountsRepo->upsertAccount(std::move(parsedBody));

        if (app::error::abortIfAppError(res, &accountDb))
        {
            return;
        }

        auto &&accountDbCasted = std::get<app::accounts::AccountDb>(std::move(accountDb));

        res
            ->writeHeader("Content-Type", "application/json")
            ->end(boost::json::serialize(std::move(boost::json::value_from(
                std::move(accountDbCasted)))));
    };

    app::utils::RequestJsonBodyReader reader;
    reader.read<app::accounts::AccountDb>(_res, _req, handler);
}
