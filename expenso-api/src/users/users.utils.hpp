#pragma once

#include <boost/json.hpp>

#include "users.models.hpp"

namespace app
{
    namespace users
    {
        boost::json::object toJsonUserDb(app::users::UserDb user);
        boost::json::object toJsonUserSettingsDb(app::users::UserSettingsDb userSettings);
    }
}