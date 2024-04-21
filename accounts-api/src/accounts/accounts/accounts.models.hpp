#pragma once

#include <string>
#include <optional>
#include <vector>

#include <boost/describe.hpp>

#include "accounts/shared/types.hpp"
#include "accounts/shared/models.hpp"

namespace accounts
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

    struct UpsertAccountDto
    {
        std::string userId;
        std::string currencyCode;
        std::string category;
        std::string name;
        std::vector<AccountDetailDb> details;
        AccountAddress address;
        double total;
    };
    BOOST_DESCRIBE_STRUCT(
        UpsertAccountDto,
        (),
        (userId, currencyCode, category, name, details, address, total))

    struct AccountDb : public UpsertAccountDto, public BaseDb
    {
    };
    BOOST_DESCRIBE_STRUCT(AccountDb, (UpsertAccountDto, BaseDb), ())
}
