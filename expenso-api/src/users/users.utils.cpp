#include "users.utils.hpp"
#include "utils/date.hpp"

boost::json::object app::users::toJsonUserDb(app::users::UserDb &&user)
{
    return boost::json::object(
        {
            {"alias", std::move(user.alias)},
            {"email", std::move(user.email)},
            {"id", std::move(user.id)},
            {"firstName", std::move(user.firstName)},
            {"lastName", std::move(user.lastName)},
            {"googleId", std::move(user.googleId)},
            {"createdAt", app::utils::timePointToString(user.createdAt)},
            {"settings", std::move(boost::json::object({{"defaultCurrency", user.settings.defaultCurrency}}))},
        });
}
