#pragma once

#include <string_view>
#include <boost/uuid/uuid.hpp>
#include <chrono>

namespace app
{
    namespace user
    {
        struct UserSettingsDb
        {
            std::string_view defaultCurrency;
        };

        struct UserDb
        {
            std::string_view id;
            std::chrono::system_clock::time_point createdAt;
            std::chrono::system_clock::time_point updatedAt;
            std::string_view firstName;
            std::string_view lastName;

            std::string_view alias;
            std::string_view email;

            std::string_view googleId;
            UserSettingsDb settings;
        };

        // struct UpdateUserDb
        // {
        //     FirstName string `gorm : "type:varchar(100);"` LastName string `gorm : "type:varchar(100);"` Alias string `gorm : "type:varchar(100);"`
        // }

    }
}
