#include "users.handlers.hpp"
#include "users.utils.hpp"
#include "services/app-dependencies.hpp"
#include "utils/error.hpp"
#include "boost/pfr.hpp"

void app::users::handleGetUserById(uWS::HttpResponse<false> *res, uWS::HttpRequest *req)
{
    auto authToken = req->getHeader("authorization");
    auto userId = req->getParameter("userId");

    auto &&authUser = app::services::AppDependencies::instance().jwtService->getAuthUser(authToken);

    if (app::error::abortIfUnauthorized(res, authUser))
    {
        return;
    }

    if (userId != authUser.value().userId)
    {
        app::error::abort(res, boost::json::object({{"message", "Only can update yourself"}}));
        return;
    }

    auto &&userDb = app::services::AppDependencies::instance().userRepo->getUserById(userId);

    if (app::error::abortIfAppError(res, &userDb))
    {
        return;
    }

    auto &&userDbCasted = std::get<app::users::UserDb>(userDb);

    res
        ->writeHeader("Content-Type", "application/json")
        ->end(boost::json::serialize(std::move(app::users::toJsonUserDb(userDbCasted))));
}
