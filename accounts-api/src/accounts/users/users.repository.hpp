#pragma once

#include <string_view>

#include <bsoncxx/builder/basic/document.hpp>

#include "accounts/shared/models.hpp"
#include "accounts/services/mongoAccess.hpp"

namespace accounts
{
    using namespace bsoncxx::v_noabi;

    class IUsersRepository
    {

    public:
        virtual AccountsResult<UserDb> getUserByGoogleId(std::string_view googleId) = 0;
        virtual AccountsResult<UserDb> getUserById(std::string_view userId) = 0;
        virtual AccountsResult<UserDb> upsertUserByIdIfNotExist(std::string_view userId, UpsertUserDb user) = 0;
        virtual AccountsResult<UserDb> createUserByGoogleIdIfNotExist(GoogleUpsertUserDb user) = 0;
    };

    class UsersRepositoryImpl : public IUsersRepository
    {
    public:
        UsersRepositoryImpl(std::shared_ptr<IMongoAccess> mongoAccess);

        virtual AccountsResult<UserDb> getUserByGoogleId(std::string_view googleId) override;
        virtual AccountsResult<UserDb> getUserById(std::string_view userId) override;
        virtual AccountsResult<UserDb> upsertUserByIdIfNotExist(std::string_view userId, UpsertUserDb user) override;
        virtual AccountsResult<UserDb> createUserByGoogleIdIfNotExist(GoogleUpsertUserDb user) override;

    private:
        AccountsResult<UserDb> getUserByQuery(bsoncxx::document::value query);

        std::shared_ptr<IMongoAccess> _mongoAccess;

        constexpr static const char *collectionName = "users";
    };
}
