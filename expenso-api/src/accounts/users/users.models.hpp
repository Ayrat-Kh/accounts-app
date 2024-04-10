#pragma once

#include <string>
#include <optional>

#include <boost/describe.hpp>

#include "accounts/shared/types.hpp"

namespace accounts::users
{

    struct UserSettingsDb
    {
        std::string defaultCurrency;
    };
    BOOST_DESCRIBE_STRUCT(UserSettingsDb, (), (defaultCurrency))

    struct UserDb
    {
        std::string id;
        ::accounts::shared::Datetime createdAt;
        std::optional<::accounts::shared::Datetime> updatedAt;
        std::string firstName;
        std::string lastName;
        std::string alias;
        std::string email;
        std::string googleId;
        UserSettingsDb settings;
    };
    BOOST_DESCRIBE_STRUCT(
        UserDb,
        (),
        (id, createdAt,
         updatedAt,
         firstName, lastName, alias, email, googleId, settings))

}
