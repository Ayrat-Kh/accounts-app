#pragma once

#include <variant>
#include <string_view>

#include <bsoncxx/builder/basic/document.hpp>

#include "shared/models.hpp"
#include "services/mongo-access.hpp"
#include "users/users.models.hpp"

namespace app::users
{
    using namespace bsoncxx::v_noabi;

    class IUsersRepository
    {
    public:
        virtual std::variant<UserDb, app::shared::AppError> getUserByGoogleId(std::string_view googleId) = 0;
        virtual std::variant<UserDb, app::shared::AppError> getUserById(std::string_view userId) = 0;
        virtual std::variant<UserDb, app::shared::AppError> createUserByIdIfNotExist(UserDb user) = 0;
        virtual std::variant<UserDb, app::shared::AppError> createUserByGoogleIdIfNotExist(UserDb user) = 0;
    };

    class UsersRepositoryImpl : public IUsersRepository
    {
    public:
        UsersRepositoryImpl(std::shared_ptr<app::services::IMongoAccess> mongoAccess);

        virtual std::variant<UserDb, app::shared::AppError> getUserByGoogleId(std::string_view googleId) override;
        virtual std::variant<UserDb, app::shared::AppError> getUserById(std::string_view userId) override;
        virtual std::variant<UserDb, app::shared::AppError> createUserByIdIfNotExist(UserDb user) override;
        virtual std::variant<UserDb, app::shared::AppError> createUserByGoogleIdIfNotExist(UserDb user) override;

    private:
        std::variant<UserDb, app::shared::AppError> getUserByQuery(bsoncxx::document::value query);
        std::variant<UserDb, app::shared::AppError> createUserByQueryIfNotExist(bsoncxx::document::value query, const UserDb &user);

        std::shared_ptr<app::services::IMongoAccess> _mongoAccess;
    };
}
