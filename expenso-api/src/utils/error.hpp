#pragma once

#include <variant>
#include <iostream>

namespace app
{
    namespace error
    {
        enum class AppErrorCode
        {
            APP_ERROR,
            INVALID_INPUT,

            DB_NOT_FOUND = 50,
            DB_INSERT_ERROR,
            DB_QUERY_ERROR,

            THIRD_PARTY_REQUEST = 100
        };

        struct AppError
        {
            std::string message;

            AppErrorCode code;
        };

        template <class... Args>
        AppError *isError(std::variant<Args...> args)
        {
            return std::get_if<AppError>(&args);
        }

        template <class... Args>
        AppError *logIfError(std::variant<Args...> args)
        {
            AppError *ptr = isError(args);

            if (ptr != nullptr)
            {

                std::cerr << "Caught error: \"{ \"code\": " << static_cast<int>(ptr->code) << ", \"message\":" << ptr->message << "}\"'\n";
            }

            return ptr;
        }
    }
}