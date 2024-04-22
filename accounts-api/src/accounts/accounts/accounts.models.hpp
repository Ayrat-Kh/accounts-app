#pragma once

#include <string>
#include <optional>
#include <vector>

#include <boost/describe.hpp>

#include "accounts/shared/types.hpp"
#include "accounts/shared/models.hpp"

namespace accounts
{
    enum class EAccountCategory
    {
        OTHER,
        GROCERY,
        SHOPPING
    };
    BOOST_DESCRIBE_ENUM(EAccountCategory, OTHER, GROCERY, SHOPPING)

    enum class EAccountStatus
    {
        DRAFT,
        CREATED
    };
    BOOST_DESCRIBE_ENUM(EAccountStatus, DRAFT, CREATED)

    struct AccountDetailDb
    {
        std::string name;
        double value;
        ECurrency currencyCode;
    };
    BOOST_DESCRIBE_STRUCT(AccountDetailDb, (), (name, value, currencyCode))

    struct AccountAddress
    {
        std::string address;
        double longitude;
        double latitude;
    };
    BOOST_DESCRIBE_STRUCT(AccountAddress, (), (address, longitude, latitude))

    struct UpsertAccountDto
    {
        Datetime date;
        std::string userId;
        ECurrency currencyCode;
        EAccountCategory category;
        std::string name;
        std::vector<AccountDetailDb> details;
        AccountAddress address;
        EAccountStatus status;
        double value;
    };
    BOOST_DESCRIBE_STRUCT(
        UpsertAccountDto,
        (),
        (userId, currencyCode, category, name, details, address, value))

    struct AccountDb : public UpsertAccountDto, public BaseDb
    {
    };
    BOOST_DESCRIBE_STRUCT(AccountDb, (UpsertAccountDto, BaseDb), ())
}
