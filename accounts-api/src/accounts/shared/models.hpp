#pragma once

#include <string>

#include <boost/json/value.hpp>
#include <boost/describe/class.hpp>
#include <boost/describe/enum.hpp>

#include "accounts/shared/types.hpp"

namespace accounts::shared
{
    enum class AppErrorCode
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
    BOOST_DESCRIBE_ENUM(AppErrorCode, APP_ERROR, INVALID_INPUT, DB_NOT_FOUND, DB_INSERT_ERROR, DB_QUERY_ERROR, THIRD_PARTY_REQUEST, VALIDATION_ERROR, PARSE_BODY_ERROR);

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
        (id,
         createdAt,
         updatedAt,
         firstName, lastName, alias, email, googleId, settings));

    // below
    // auth
    struct AuthUser
    {
        std::string userId;
    };
}
