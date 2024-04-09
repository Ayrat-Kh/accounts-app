#pragma once

#include <variant>
#include <string_view>
#include <vector>

#include <bsoncxx/builder/basic/document.hpp>

#include "shared/models.hpp"
#include "utils/error.hpp"
#include "services/mongoAccess.hpp"
#include "accounts/accounts.models.hpp"

namespace app
{
    namespace accounts
    {
        using namespace bsoncxx::v_noabi;

        class IAccountsRepository
        {
        public:
            virtual std::variant<AccountDb, app::shared::AppError> getAccountById(std::string_view accountId) = 0;
            virtual std::variant<std::vector<AccountDb>, app::shared::AppError> getAccountsByUserId(std::string_view userId) = 0;
            virtual std::variant<AccountDb, app::shared::AppError> upsertAccount(AccountDb account) = 0;
        };

        class AccountsRepositoryImpl : public IAccountsRepository
        {
        public:
            AccountsRepositoryImpl(std::shared_ptr<app::services::IMongoAccess> mongoAccess);

            virtual std::variant<AccountDb, app::shared::AppError> getAccountById(std::string_view accountId) override;
            virtual std::variant<std::vector<AccountDb>, app::shared::AppError> getAccountsByUserId(std::string_view userId) override;
            virtual std::variant<AccountDb, app::shared::AppError> upsertAccount(AccountDb account) override;

        private:
            std::shared_ptr<app::services::IMongoAccess> _mongoAccess;
        };

    }
}
