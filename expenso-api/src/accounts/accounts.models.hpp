#pragma once

#include <string>
#include <optional>
#include <vector>

#include <boost/describe.hpp>

#include "shared/types.hpp"

namespace app::accounts
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
        app::shared::Datetime createdAt;
        std::optional<app::shared::Datetime> updatedAt;
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
