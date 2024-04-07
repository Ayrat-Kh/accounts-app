#pragma once

#include <variant>
#include <string_view>

#include <bsoncxx/builder/basic/document.hpp>

#include "utils/error.hpp"
#include "services/mongo-access.hpp"
#include "users/users.models.hpp"

namespace app
{
    namespace users
    {
        using namespace bsoncxx::v_noabi;

        class IUsersRepository
        {
        public:
            virtual std::variant<UserDb, app::error::AppError> getUserByGoogleId(std::string_view googleId) = 0;
            virtual std::variant<UserDb, app::error::AppError> getUserById(std::string_view userId) = 0;
            virtual std::variant<UserDb, app::error::AppError> createUserByIdIfNotExist(UserDb user) = 0;
            virtual std::variant<UserDb, app::error::AppError> createUserByGoogleIdIfNotExist(UserDb user) = 0;
        };

        class UsersRepositoryImpl : public IUsersRepository
        {
        public:
            UsersRepositoryImpl(std::shared_ptr<app::services::IMongoAccess> mongoAccess);

            virtual std::variant<UserDb, app::error::AppError> getUserByGoogleId(std::string_view googleId) override;
            virtual std::variant<UserDb, app::error::AppError> getUserById(std::string_view userId) override;
            virtual std::variant<UserDb, app::error::AppError> createUserByIdIfNotExist(UserDb user) override;
            virtual std::variant<UserDb, app::error::AppError> createUserByGoogleIdIfNotExist(UserDb user) override;

        private:
            void fillUserDb(bsoncxx::document::value &v, UserDb &userDb);
            std::variant<UserDb, app::error::AppError> getUserByQuery(bsoncxx::document::value query);
            std::variant<UserDb, app::error::AppError> createUserByQueryIfNotExist(bsoncxx::document::value query, const UserDb &user);

            std::shared_ptr<app::services::IMongoAccess> _mongoAccess;
        };

    }
}
