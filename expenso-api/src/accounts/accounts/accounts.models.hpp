#pragma once

#include <string>
#include <optional>
#include <vector>

#include <boost/describe.hpp>

#include "accounts/shared/types.hpp"

namespace accounts::accounts
{
    struct AccountDetailDb
    {
        std::string name;
        double price;
        std::string currencyCode;
    };
    BOOST_DESCRIBE_STRUCT(AccountDetailDb, (), (name, price, currencyCode))

    struct AccountAddress
    {
        std::string address;
        double longitude;
        double latitude;
    };
    BOOST_DESCRIBE_STRUCT(AccountAddress, (), (address, longitude, latitude))

    struct AccountDb
    {
        std::string id;
        shared::Datetime createdAt;
        std::optional<shared::Datetime> updatedAt;
        std::string userId;
        std::string currencyCode;
        std::string category;
        std::string name;
        std::vector<AccountDetailDb> details;
        AccountAddress address;

        double total;
    };
    BOOST_DESCRIBE_STRUCT(
        AccountDb,
        (),
        (id,
         createdAt,
         updatedAt,
         userId, currencyCode, category, name, details, address, total))
}
