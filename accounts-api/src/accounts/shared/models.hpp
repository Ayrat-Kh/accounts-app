#pragma once

#include <string>

#include <boost/system/result.hpp>
#include <boost/json/value.hpp>
#include <boost/describe/class.hpp>
#include <boost/describe/enum.hpp>

#include "accounts/shared/types.hpp"

namespace accounts
{
    enum class ECurrency
    {
        USD,
        EUR,
    };
    BOOST_DESCRIBE_ENUM(ECurrency, USD, EUR)

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

    template <class T>
    using AccountsResult = boost::system::result<T, AppError>;

    BOOST_NORETURN BOOST_NOINLINE inline void throw_exception_from_error(AppError const &e, boost::source_location const &loc)
    {
        boost::throw_with_location(std::domain_error(e.code), loc);
    }

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
        ECurrency defaultCurrency;
    };
    BOOST_DESCRIBE_STRUCT(UserSettingsDb, (), (defaultCurrency));

    enum class EUserStatus
    {
        CREATED,
        BLOCKED,
    };
    BOOST_DESCRIBE_ENUM(EUserStatus, CREATED, BLOCKED)

    struct UpsertUserDb
    {
        std::string firstName;
        std::string lastName;
        std::string alias;
        std::string email;
        std::string googleId;
        EUserStatus status;
        UserSettingsDb settings;
        std::optional<Datetime> updatedAt;
    };
    BOOST_DESCRIBE_STRUCT(
        UpsertUserDb,
        (),
        (updatedAt, firstName, lastName, alias, email, googleId, status, settings));

    struct GoogleUpsertUserDb : public UpsertUserDb
    {
        std::string id;
    };
    BOOST_DESCRIBE_STRUCT(
        GoogleUpsertUserDb,
        (UpsertUserDb),
        (id, updatedAt, firstName, lastName, alias, email, googleId, status, settings));

    struct UserDb : public GoogleUpsertUserDb
    {
        std::string firstName;
        std::string lastName;
        std::string alias;
        std::string email;
        std::string googleId;
        EUserStatus status;
        UserSettingsDb settings;
        Datetime createdAt;
    };
    BOOST_DESCRIBE_STRUCT(
        UserDb,
        (GoogleUpsertUserDb),
        (id, createdAt, updatedAt, firstName, lastName, alias, email, googleId, status, settings));

    // below
    // auth
    struct AuthUser
    {
        std::string userId;
    };
    BOOST_DESCRIBE_STRUCT(AuthUser, (), (userId));

    // below config model
    struct EnvironmentVar
    {
        std::int32_t port;
        std::string mongoUrl;
        std::string jwtKey;
        std::string dbName;
        std::string geoCodeApiKey;
    };
}
