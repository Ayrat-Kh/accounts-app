#pragma once

#include <string>
#include <optional>

#include <boost/uuid/uuid.hpp>
#include <chrono>

namespace app
{
    namespace users
    {
        struct UserSettingsDb
        {
            std::string defaultCurrency;
        };

        struct UserDb
        {
            std::string id;
            std::chrono::system_clock::time_point createdAt;
            std::optional<std::chrono::system_clock::time_point> updatedAt;
            std::string firstName;
            std::string lastName;
            std::string alias;
            std::string email;
            std::string googleId;
            UserSettingsDb settings;
        };
    }
}
