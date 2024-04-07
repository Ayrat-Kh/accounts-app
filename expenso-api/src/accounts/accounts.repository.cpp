#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "accounts.repository.hpp"
#include "utils/date.hpp"

using namespace app::accounts;
using namespace app::services;
using namespace bsoncxx::v_noabi;

using bsoncxx::builder::basic::kvp;

using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

// // UsersRepositoryImpl::UsersRepositoryImpl(std::shared_ptr<app::services::IMongoAccess> mongoAccess)
// //     : _mongoAccess(mongoAccess)
// // {
// // }

// // std::variant<UserDb, app::error::AppError> UsersRepositoryImpl::getUserByGoogleId(std::string_view googleId)
// // {
// //     return std::move(
// //         getUserByQuery(
// //             std::move(
// //                 make_document(kvp("googleId", googleId)))));
// // }

// // std::variant<UserDb, app::error::AppError> UsersRepositoryImpl::getUserById(std::string_view userId)
// // {
// //     return std::move(
// //         getUserByQuery(
// //             std::move(
// //                 make_document(kvp("_id", userId)))));
// // }

// // std::variant<UserDb, app::error::AppError> UsersRepositoryImpl::createUserByIdIfNotExist(UserDb user)
// // {
// //     return std::move(
// //         createUserByQueryIfNotExist(
// //             std::move(
// //                 make_document(
// //                     kvp("_id", user.id))),
// //             user));
// // }

// // std::variant<UserDb, app::error::AppError> UsersRepositoryImpl::createUserByGoogleIdIfNotExist(UserDb user)
// // {
// //     return std::move(
// //         createUserByQueryIfNotExist(
// //             std::move(
// //                 make_document(
// //                     kvp("googleId", user.googleId))),
// //             user));
// // }

// // void app::users::UsersRepositoryImpl::fillUserDb(bsoncxx::document::value &userDocument, UserDb &userDb)
// // {
// //     if (userDocument["firstName"] && userDocument["firstName"].type() == bsoncxx::type::k_string)
// //     {
// //         userDb.firstName = std::string(userDocument["firstName"].get_string().value);
// //     }

// //     if (userDocument["lastName"] && userDocument["lastName"].type() == bsoncxx::type::k_string)
// //     {
// //         userDb.lastName = std::string(userDocument["lastName"].get_string().value);
// //     }

// //     if (userDocument["email"] && userDocument["email"].type() == bsoncxx::type::k_string)
// //     {
// //         userDb.email = std::string(userDocument["email"].get_string().value);
// //     }

// //     if (userDocument["googleId"] && userDocument["googleId"].type() == bsoncxx::type::k_string)
// //     {
// //         userDb.googleId = std::string(userDocument["googleId"].get_string().value);
// //     }

// //     if (userDocument["alias"] && userDocument["alias"].type() == bsoncxx::type::k_string)
// //     {
// //         userDb.alias = std::string(userDocument["alias"].get_string().value);
// //     }

// //     if (userDocument["_id"] && userDocument["_id"].type() == bsoncxx::type::k_string)
// //     {
// //         userDb.id = std::string(userDocument["_id"].get_string().value);
// //     }

// //     if (userDocument["createdAt"] && userDocument["createdAt"].type() == bsoncxx::type::k_date)
// //     {

// //         // Extract the milliseconds value
// //         std::int64_t milliseconds = userDocument["createdAt"].get_date().to_int64();

// //         userDb.createdAt = std::chrono::system_clock::from_time_t(0) + std::chrono::milliseconds(milliseconds);
// //     }

// //     if (userDocument["settings"] && userDocument["settings"].type() == bsoncxx::type::k_document)
// //     {
// //         auto doc = userDocument["settings"].get_document().value;

// //         userDb.settings.defaultCurrency = std::string(doc["defaultCurrency"].get_string().value);
// //     }
// // }

// // std::variant<UserDb, app::error::AppError> app::users::UsersRepositoryImpl::getUserByQuery(bsoncxx::document::value query)
// // {
// //     auto client = _mongoAccess->getConnection();
// //     auto db = (*client)["budget"];

// //     try
// //     {
// //         stdx::optional<document::value> result = db.collection("users").find_one(query.view());

// //         if (!result.has_value())
// //         {
// //             return std::move(
// //                 app::error::AppError{
// //                     .message = "Document not found",
// //                     .code = app::error::AppErrorCode::DB_NOT_FOUND});
// //         }

// //         UserDb userDb = {};

// //         fillUserDb(result.value(), userDb);

// //         return std::move(userDb);
// //     }
// //     catch (std::exception &exception)
// //     {
// //         return std::move(
// //             app::error::AppError{
// //                 .message = exception.what(),
// //                 .code = app::error::AppErrorCode::DB_QUERY_ERROR});
// //     }
// // }

// // std::variant<UserDb, app::error::AppError> app::users::UsersRepositoryImpl::createUserByQueryIfNotExist(bsoncxx::document::value query, const UserDb &user)
// // {
// //     auto client = _mongoAccess->getConnection();
// //     auto db = (*client)["expenso-app"];

// //     try
// //     {

// //         bsoncxx::document::value insertData = make_document(
// //             kvp("_id", user.id),
// //             kvp("firstName", user.firstName),
// //             kvp("alias", user.alias),
// //             kvp("email", user.email),
// //             kvp("googleId", user.googleId),
// //             kvp("lastName", user.lastName),
// //             kvp("createdAt", bsoncxx::types::b_date{std::chrono::system_clock::now()}),
// //             kvp("updatedAt", bsoncxx::types::b_null{}),
// //             kvp("settings", make_document(
// //                                 kvp("defaultCurrency", user.settings.defaultCurrency))));

// //         mongocxx::options::find_one_and_update options;
// //         options.upsert(true);
// //         options.return_document(mongocxx::options::return_document::k_after);

// //         stdx::optional<bsoncxx::document::value> result =
// //             db
// //                 .collection("users")
// //                 .find_one_and_update(
// //                     query.view(),
// //                     make_document(kvp("$setOnInsert", insertData.view())),
// //                     options);

// //         if (!result.has_value())
// //         {
// //             return std::move(
// //                 app::error::AppError{
// //                     .message = "Couldn't insert document",
// //                     .code = app::error::AppErrorCode::DB_INSERT_ERROR});
// //         }

// //         UserDb userDb = {};

// //         fillUserDb(result.value(), userDb);

// //         return std::move(userDb);
// //     }
// //     catch (std::exception &exception)
// //     {
// //         return std::move(
// //             app::error::AppError{
// //                 .message = exception.what(),
// //                 .code = app::error::AppErrorCode::DB_INSERT_ERROR});
// //     }
// // }

// app::accounts::AccountsRepositoryImpl::AccountsRepositoryImpl(std::shared_ptr<app::services::IMongoAccess> mongoAccess)
// {
// }

// std::variant<AccountDb, app::error::AppError> app::accounts::AccountsRepositoryImpl::getAccountById(std::string_view accountId)
// {
//     return std::variant<AccountDb, app::error::AppError>();
// }

// std::variant<std::vector<AccountDb>, app::error::AppError> app::accounts::AccountsRepositoryImpl::getAccountsByUserId(std::string_view userId)
// {
//     return std::variant<std::vector<AccountDb>, app::error::AppError>();
// }

// std::variant<AccountDb, app::error::AppError> app::accounts::AccountsRepositoryImpl::upsertAccount(AccountDb account)
// {
//     return std::variant<AccountDb, app::error::AppError>();
// }

void app::accounts::AccountsRepositoryImpl::fillAccountDb(bsoncxx::document::value &document, AccountDb &account)
{
    if (document["category"] && document["category"].type() == bsoncxx::type::k_string)
    {
        account.category = std::string(document["category"].get_string().value);
    }

    if (document["currencyCode"] && document["currencyCode"].type() == bsoncxx::type::k_string)
    {
        account.currencyCode = std::string(document["currencyCode"].get_string().value);
    }

    if (document["name"] && document["name"].type() == bsoncxx::type::k_string)
    {
        account.name = std::string(document["name"].get_string().value);
    }

    if (document["category"] && document["category"].type() == bsoncxx::type::k_string)
    {
        account.category = std::string(document["category"].get_string().value);
    }

    if (document["createdAt"] && document["createdAt"].type() == bsoncxx::type::k_string)
    {
        account.createdAt = app::utils::msToTimePoint(document["createdAt"].get_date().to_int64());
    }
    if (document["updatedAt"] && document["updatedAt"].type() == bsoncxx::type::k_date)
    {
        account.updatedAt = app::utils::msToTimePoint(document["updatedAt"].get_date().to_int64());
    }

    if (document["_id"] && document["_id"].type() == bsoncxx::type::k_string)
    {
        account.id = std::string(document["_id"].get_string().value);
    }

    if (document["userId"] && document["userId"].type() == bsoncxx::type::k_string)
    {
        account.userId = std::string(document["userId"].get_string().value);
    }

    if (document["total"] && document["total"].type() == bsoncxx::type::k_double)
    {
        account.total = document["total"].get_double();
    }

    if (document["details"] && document["details"].type() == bsoncxx::type::k_array)
    {
        auto &&details = document["details"].get_array().value;

        for (auto &&detailsItem : details)
        {
            if (detailsItem.type() != bsoncxx::type::k_document)
            {
                continue;
            }
            auto &&detailItemDocument = detailsItem.get_document().value;

            app::accounts::AccountDetailDb &&detail = {};

            if (detailItemDocument["price"] && detailItemDocument["price"].type() == bsoncxx::type::k_double)
            {
                detail.price = detailItemDocument["price"].get_double();
            }

            if (detailItemDocument["currencyCode"] && detailItemDocument["currencyCode"].type() == bsoncxx::type::k_double)
            {
                detail.currencyCode = std::string(detailItemDocument["currencyCode"].get_string().value);
            }

            if (detailItemDocument["name"] && detailItemDocument["name"].type() == bsoncxx::type::k_double)
            {
                detail.name = std::string(detailItemDocument["name"].get_string().value);
            }

            account.details.push_back(detail);
        }
    }

    // if (document["address"] && document["address"].type() == bsoncxx::type::k_document)
    // {
    //     account.address auto &&documentAddress = document["details"].app::accounts::AccountAddress &&address = {};

    //     if (detailItemDocument["price"] && detailItemDocument["price"].type() == bsoncxx::type::k_double)
    //     {
    //         detail.price = detailItemDocument["price"].get_double();
    //     }

    //     if (detailItemDocument["currencyCode"] && detailItemDocument["currencyCode"].type() == bsoncxx::type::k_double)
    //     {
    //         detail.currencyCode = std::string(detailItemDocument["currencyCode"].get_string().value);
    //     }

    //     if (detailItemDocument["name"] && detailItemDocument["name"].type() == bsoncxx::type::k_double)
    //     {
    //         detail.name = std::string(detailItemDocument["name"].get_string().value);
    //     }

    //     account.address.push_back(detail);

    //     auto &&details = document["details"].get_array().value;

    //     for (auto &&detailsItem : details)
    //     {
    //         if (detailsItem.type() != bsoncxx::type::k_document)
    //         {
    //             continue;
    //         }
    //         auto &&detailItemDocument = detailsItem.get_document().value;

    //         app::accounts::AccountAddress &&detail = {};

    //         if (detailItemDocument["price"] && detailItemDocument["price"].type() == bsoncxx::type::k_double)
    //         {
    //             detail.price = detailItemDocument["price"].get_double();
    //         }

    //         if (detailItemDocument["currencyCode"] && detailItemDocument["currencyCode"].type() == bsoncxx::type::k_double)
    //         {
    //             detail.currencyCode = std::string(detailItemDocument["currencyCode"].get_string().value);
    //         }

    //         if (detailItemDocument["name"] && detailItemDocument["name"].type() == bsoncxx::type::k_double)
    //         {
    //             detail.name = std::string(detailItemDocument["name"].get_string().value);
    //         }
    //     }
    // }
}
