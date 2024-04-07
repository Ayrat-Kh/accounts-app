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

        /*
        example:
        {
            "id": "1",
            createdAt: "2024/01/02T19:00:00",
            updatedAt: "2024/01/02T19:00:00",
        }

        */
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
