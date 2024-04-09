#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "accounts.repository.hpp"
#include "utils/date.hpp"
#include "utils/enumToString.hpp"
#include "utils/mongoDocument.hpp"
#include "utils/error.hpp"

using namespace app::accounts;
using namespace app::services;
using namespace bsoncxx::v_noabi;

using bsoncxx::builder::basic::kvp;

using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

app::accounts::AccountsRepositoryImpl::AccountsRepositoryImpl(std::shared_ptr<app::services::IMongoAccess> mongoAccess)
    : _mongoAccess(mongoAccess)
{
}

std::variant<AccountDb, app::shared::AppError> app::accounts::AccountsRepositoryImpl::getAccountById(std::string_view accountId)
{
    return std::variant<AccountDb, app::shared::AppError>();
}

std::variant<std::vector<AccountDb>, app::shared::AppError> app::accounts::AccountsRepositoryImpl::getAccountsByUserId(std::string_view userId)
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

        auto count = 1;
        for (auto doc : accountsAll)
        {
            count += 1;
        }
        result.reserve(count);

        for (auto doc : accountsAll)
        {
            result.push_back(app::utils::deserializeMongoDocument<AccountDb>(doc));
        }

        return result;
    }
    catch (std::exception &exception)
    {
        return std::move(
            app::shared::AppError{
                .message = "Couldn't fetch user accounts " + std::string(exception.what()),
                .code = app::utils::enumToString(app::shared::AppErrorCode::DB_QUERY_ERROR)});
    }
}

std::variant<app::accounts::AccountDb, app::shared::AppError> app::accounts::AccountsRepositoryImpl::upsertAccount(app::accounts::AccountDb account)
{
    auto client = _mongoAccess->getConnection();
    auto db = (*client)["expenso-app"];

    try
    {
        std::string id = account.id;
        bsoncxx::builder::basic::document insertData = app::utils::toMongoDocument(std::move(account));

        mongocxx::options::find_one_and_update options;
        options.upsert(true);
        options.return_document(mongocxx::options::return_document::k_after);

        stdx::optional<bsoncxx::document::value> result =
            db
                .collection("accounts")
                .find_one_and_update(
                    make_document(kvp("_id", id)),
                    make_document(kvp("$setOnInsert", insertData.view())),
                    options);

        if (!result.has_value())
        {
            return std::move(
                app::shared::AppError{
                    .message = "Couldn't insert account",
                    .code = app::utils::enumToString(app::shared::AppErrorCode::DB_INSERT_ERROR)});
        }

        return std::move(app::utils::deserializeMongoDocument<AccountDb>(std::move(result.value())));
    }
    catch (std::exception &exception)
    {
        return std::move(
            app::shared::AppError{
                .message = "Couldn't insert account " + std::string(exception.what()),
                .code = app::utils::enumToString(app::shared::AppErrorCode::DB_INSERT_ERROR)});
    }
}
