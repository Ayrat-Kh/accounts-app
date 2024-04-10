#pragma once

#include <string>

#include <boost/json.hpp>
#include <boost/describe.hpp>

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
    BOOST_DESCRIBE_ENUM(AppErrorCode, APP_ERROR, INVALID_INPUT, DB_NOT_FOUND, DB_INSERT_ERROR, DB_QUERY_ERROR, THIRD_PARTY_REQUEST, VALIDATION_ERROR, PARSE_BODY_ERROR)

    struct AppError
    {
        std::string message;

        const char *code;
    };
    BOOST_DESCRIBE_STRUCT(
        AppError,
        (),
        (message, code))

    struct ValidationAppError
    {
        boost::json::value errors;

        std::string message;

        const char *code;
    };
    BOOST_DESCRIBE_STRUCT(
        ValidationAppError,
        (),
        (message, code, errors))
}