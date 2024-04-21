#pragma once

#include <string>

#include <boost/json/value.hpp>
#include <boost/describe/class.hpp>
#include <boost/describe/enum.hpp>

#include "accounts/shared/types.hpp"

namespace accounts
{
    struct BaseDb
    {
        std::string id;
        Datetime createdAt;
        std::optional<Datetime> updatedAt;
    };
    BOOST_DESCRIBE_STRUCT(BaseDb, (), (id, createdAt, updatedAt));

    enum class EAppErrorCode
    {
        APP_ERROR,
        INVALID_INPUT,

        DB_NOT_FOUND = 50,
        DB_INSERT_ERROR,
        DB_QUERY_ERROR,

        THIRD_PARTY_REQUEST = 100,

        VALIDATION_ERROR = 400,
        PARSE_BODY_ERROR
    };
    BOOST_DESCRIBE_ENUM(EAppErrorCode, APP_ERROR, INVALID_INPUT, DB_NOT_FOUND, DB_INSERT_ERROR, DB_QUERY_ERROR, THIRD_PARTY_REQUEST, VALIDATION_ERROR, PARSE_BODY_ERROR);

    struct AppError
    {
        const char *code;

        std::string message;
    };
    BOOST_DESCRIBE_STRUCT(
        AppError,
        (),
        (message, code));

    struct ValidationAppError
    {
        ::boost::json::value errors;

        const char *code;

        std::string message;
    };
    BOOST_DESCRIBE_STRUCT(
        ValidationAppError,
        (),
        (message, code, errors))

    // below
    // users
    struct UserSettingsDb
    {
        std::string defaultCurrency;
    };
    BOOST_DESCRIBE_STRUCT(UserSettingsDb, (), (defaultCurrency));

    enum class EUserStatus
    {
        CREATED,
        BLOCKED,
    };
    BOOST_DESCRIBE_ENUM(EUserStatus, CREATED, BLOCKED)

    struct UserDb : public BaseDb
    {
        std::string firstName;
        std::string lastName;
        std::string alias;
        std::string email;
        std::string googleId;
        EUserStatus status;
        UserSettingsDb settings;
    };
    BOOST_DESCRIBE_STRUCT(
        UserDb,
        (BaseDb),
        (firstName, lastName, alias, email, googleId, status, settings));

    // below
    // auth
    struct AuthUser
    {
        std::string userId;
    };
    BOOST_DESCRIBE_STRUCT(AuthUser, (), (userId));
}
