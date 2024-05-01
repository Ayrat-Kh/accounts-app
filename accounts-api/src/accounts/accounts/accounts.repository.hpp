#pragma once

#include <string_view>
#include <vector>

#include <bsoncxx/builder/basic/document.hpp>

#include "accounts/shared/models.hpp"
#include "accounts/utils/error.hpp"
#include "accounts/services/mongoAccess.hpp"
#include "accounts/accounts/accounts.models.hpp"

namespace accounts
{
    using namespace bsoncxx::v_noabi;

    class IAccountsRepository
    {
    public:
        virtual AccountsResult<AccountDb> getAccountById(std::string_view accountId) = 0;
        virtual AccountsResult<std::vector<AccountDb>> getAccountsByUserId(std::string_view userId) = 0;
        virtual AccountsResult<AccountDb> upsertAccount(std::string_view accountId, UpsertAccountDb account) = 0;
    };

    class AccountsRepositoryImpl : public IAccountsRepository
    {
    public:
        AccountsRepositoryImpl(std::shared_ptr<IMongoAccess> mongoAccess);

        virtual AccountsResult<AccountDb> getAccountById(std::string_view accountId) override;
        virtual AccountsResult<std::vector<AccountDb>> getAccountsByUserId(std::string_view userId) override;
        virtual AccountsResult<AccountDb> upsertAccount(std::string_view accountId, UpsertAccountDb account) override;

    private:
        std::shared_ptr<IMongoAccess> _mongoAccess;

        constexpr static const char *collectionName = "accounts";
    };
}
