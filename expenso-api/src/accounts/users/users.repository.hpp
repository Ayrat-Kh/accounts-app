#pragma once

#include <variant>
#include <string_view>

#include <bsoncxx/builder/basic/document.hpp>

#include "accounts/shared/models.hpp"
#include "accounts/services/mongoAccess.hpp"

namespace accounts::users
{
    using namespace bsoncxx::v_noabi;

    class IUsersRepository
    {

    public:
        virtual std::variant<shared::UserDb, shared::AppError> getUserByGoogleId(std::string_view googleId) = 0;
        virtual std::variant<shared::UserDb, shared::AppError> getUserById(std::string_view userId) = 0;
        virtual std::variant<shared::UserDb, shared::AppError> createUserByIdIfNotExist(shared::UserDb user) = 0;
        virtual std::variant<shared::UserDb, shared::AppError> createUserByGoogleIdIfNotExist(shared::UserDb user) = 0;
    };

    class UsersRepositoryImpl : public IUsersRepository
    {
    public:
        UsersRepositoryImpl(std::shared_ptr<services::IMongoAccess> mongoAccess);

        virtual std::variant<shared::UserDb, shared::AppError> getUserByGoogleId(std::string_view googleId) override;
        virtual std::variant<shared::UserDb, shared::AppError> getUserById(std::string_view userId) override;
        virtual std::variant<shared::UserDb, shared::AppError> createUserByIdIfNotExist(shared::UserDb user) override;
        virtual std::variant<shared::UserDb, shared::AppError> createUserByGoogleIdIfNotExist(shared::UserDb user) override;

    private:
        std::variant<shared::UserDb, shared::AppError> getUserByQuery(bsoncxx::document::value query);
        std::variant<shared::UserDb, shared::AppError> createUserByQueryIfNotExist(bsoncxx::document::value query, const shared::UserDb &user);

        std::shared_ptr<services::IMongoAccess> _mongoAccess;
    };
}
