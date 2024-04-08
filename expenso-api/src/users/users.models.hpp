#pragma once

#include <string>
#include <optional>

#include <boost/describe.hpp>

#include "shared/types.hpp"

namespace app::users
{

    struct UserSettingsDb
    {
        std::string defaultCurrency;
    };
    BOOST_DESCRIBE_STRUCT(UserSettingsDb, (), (defaultCurrency))

    struct UserDb
    {
        std::string id;
        app::shared::Datetime createdAt;
        // std::optional<app::shared::Datetime> updatedAt;
        std::string firstName;
        std::string lastName;
        std::string alias;
        std::string email;
        std::string googleId;
        UserSettingsDb settings;
    };
    BOOST_DESCRIBE_STRUCT(UserDb, (), (id, createdAt,
                                       //  updatedAt,
                                       firstName, lastName, alias, email, googleId, settings))

}
