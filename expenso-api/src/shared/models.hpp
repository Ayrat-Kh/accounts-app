#pragma once

#include <string>

#include <boost/describe.hpp>

namespace app::shared
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
        PARSE_ERROR
    };
    BOOST_DESCRIBE_ENUM(AppErrorCode, APP_ERROR, INVALID_INPUT, DB_NOT_FOUND, DB_INSERT_ERROR, DB_QUERY_ERROR, THIRD_PARTY_REQUEST, VALIDATION_ERROR, PARSE_ERROR)

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
        std::string errors;

        std::string message;

        const char *code;
    };
    BOOST_DESCRIBE_STRUCT(
        ValidationAppError,
        (),
        (message, code, errors))
}