#pragma once

#include <variant>
#include <string_view>
#include <exception>

#include "user.models.hpp"

namespace app
{
    namespace user
    {
        class IUserRepository
        {
        public:
            virtual std::variant<UserDb, std::exception_ptr> GetUserById(std::string_view userId) = 0;
            virtual std::variant<std::string_view, std::exception_ptr> UpsertUserById(UserDb user) = 0;
        };

        class UserRepositoryImpl : public IUserRepository
        {
        public:
            virtual std::variant<UserDb, std::exception_ptr> GetUserById(std::string_view userId);
            virtual std::variant<std::string_view, std::exception_ptr> UpsertUserById(UserDb user);
        };

    }
}
