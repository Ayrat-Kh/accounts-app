#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "accounts.repository.hpp"
#include "accounts/utils/date.hpp"
#include "accounts/utils/enumToString.hpp"
#include "accounts/utils/mongoDocument.hpp"
#include "accounts/utils/error.hpp"

using namespace ::accounts::services;
using namespace ::accounts::shared;
using namespace ::accounts::utils;
using namespace ::accounts::error;

using namespace bsoncxx::v_noabi;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

accounts::accounts::AccountsRepositoryImpl::AccountsRepositoryImpl(std::shared_ptr<IMongoAccess> mongoAccess)
    : _mongoAccess(mongoAccess)
{
}

std::variant<accounts::accounts::AccountDb, AppError> accounts::accounts::AccountsRepositoryImpl::getAccountById(std::string_view accountId)
{
    return std::variant<AccountDb, AppError>();
}

std::variant<std::vector<accounts::accounts::AccountDb>, AppError> accounts::accounts::AccountsRepositoryImpl::getAccountsByUserId(std::string_view userId)
{
    auto client = _mongoAccess->getConnection();
    auto db = (*client)["expenso-app"];

    try
    {
        auto accountsAll =
            db
                .collection("accounts")
                .find(make_document(kvp("userId", userId)));

        std::vector<AccountDb> result;

        for (auto doc : accountsAll)
        {
            result.push_back(deserializeMongoDocument<AccountDb>(doc));
        }

        return result;
    }
    catch (std::exception &exception)
    {
        return std::move(
            AppError{
                .code = enumToString(AppErrorCode::DB_QUERY_ERROR),
                .message = "Couldn't fetch user accounts " + std::string(exception.what()),
            });
    }
}

std::variant<accounts::accounts::AccountDb, AppError> accounts::accounts::AccountsRepositoryImpl::upsertAccount(AccountDb account)
{
    auto client = _mongoAccess->getConnection();
    auto db = (*client)["expenso-app"];

    try
    {
        std::string id = account.id;
        bsoncxx::builder::basic::document insertData = toMongoDocument(std::move(account));

        mongocxx::options::find_one_and_update options;
        options.upsert(true);
        options.return_document(mongocxx::options::return_document::k_after);

        stdx::optional<bsoncxx::document::value> result =
            db
                .collection("accounts")
                .find_one_and_update(
                    make_document(kvp("_id", id)),
                    insertData.view(),
                    options);

        if (!result.has_value())
        {
            return std::move(
                AppError{
                    .code = enumToString(AppErrorCode::DB_INSERT_ERROR),
                    .message = "Couldn't insert account"});
        }

        return std::move(deserializeMongoDocument<AccountDb>(std::move(result.value())));
    }
    catch (std::exception &exception)
    {
        return std::move(
            AppError{
                .code = enumToString(AppErrorCode::DB_INSERT_ERROR),
                .message = "Couldn't insert account " + std::string(exception.what())});
    }
}