#pragma once

#include <string>
#include <chrono>
#include <optional>
#include <vector>

namespace app
{
    namespace accounts
    {
        struct AccountDetailDb
        {
            std::string name;
            double price;
            std::string currencyCode;
        };

        struct AccountAddress
        {
            std::string address;
            double longitude;
            double latitude;
        };

        struct AccountDb
        {
            std::string id;
            std::chrono::system_clock::time_point createdAt;
            std::optional<std::chrono::system_clock::time_point> updatedAt;
            std::string userId;
            std::string currencyCode;
            std::string category;
            std::string name;
            std::vector<AccountDetailDb> details;
            AccountAddress address;

            double total;
        };
    }
}