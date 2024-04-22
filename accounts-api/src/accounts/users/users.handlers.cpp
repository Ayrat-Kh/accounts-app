#include <boost/json.hpp>

#include "users.handlers.hpp"
#include "accounts/services/appDependencies.hpp"
#include "accounts/utils/error.hpp"
#include "accounts/utils/jsonSerialize.hpp"
#include "accounts/utils/enumHelpers.hpp"

void accounts::handleGetUserById(uWS::HttpResponse<false> *res, uWS::HttpRequest *req)
{
    auto authToken = req->getHeader("authorization");
    auto userId = req->getParameter("userId");

    auto &&authUser = AppDependencies::instance().jwtService->getAuthUser(authToken);

    if (abortIfUnauthorized(res, authUser))
    {
        return;
    }

    if (userId != authUser.value().userId)
    {
        abort(
            res,
            std::move(
                AppError{
                    .code = enumToString(EAppErrorCode::VALIDATION_ERROR),
                    .message = "Only can fetch information about yourself"}));

        return;
    }

    auto &&userDb = AppDependencies::instance().userRepo->getUserById(userId);

    if (abortIfAppError(res, &userDb))
    {
        return;
    }

    auto &&userDbCasted = std::get<UserDb>(std::move(userDb));

    res
        ->writeHeader("Content-Type", "application/json")
        ->end(boost::json::serialize(
            boost::json::object(
                {{"user",
                  boost::json::value_from(std::move(userDbCasted))}})));
}
