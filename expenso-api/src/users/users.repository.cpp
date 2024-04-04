#include <mongocxx/client.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "services/mongo-access.hpp"
#include "users.repository.hpp"

using namespace app::users;
using namespace app::services;
using namespace bsoncxx::v_noabi;

using bsoncxx::builder::basic::kvp;

using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

std::variant<UserDb, app::error::AppError> UsersRepositoryImpl::getUserByGoogleId(std::string_view googleId)
{
    return std::move(
        getUserByQuery(
            std::move(
                make_document(kvp("googleId", googleId)))));
}

std::variant<UserDb, app::error::AppError> UsersRepositoryImpl::getUserById(std::string_view userId)
{
    return std::move(
        getUserByQuery(
            std::move(
                make_document(kvp("_id", userId)))));
}

std::variant<UserDb, app::error::AppError> UsersRepositoryImpl::createUserByIdIfNotExist(const UserDb &user)
{
    return std::move(
        createUserByQueryIfNotExist(
            std::move(
                make_document(
                    kvp("_id", user.id))),
            user));
}

std::variant<UserDb, app::error::AppError> UsersRepositoryImpl::createUserByGoogleIdIfNotExist(const UserDb &user)
{
    return std::move(
        createUserByQueryIfNotExist(
            std::move(
                make_document(
                    kvp("googleId", user.googleId))),
            user));
}

void app::users::UsersRepositoryImpl::fillUserDb(bsoncxx::document::value &userDocument, UserDb &userDb)
{
    if (userDocument["firstName"].type() == bsoncxx::type::k_string)
    {
        userDb.firstName = std::string(userDocument["firstName"].get_string().value);
    }

    if (userDocument["lastName"].type() == bsoncxx::type::k_string)
    {
        userDb.lastName = std::string(userDocument["lastName"].get_string().value);
    }

    if (userDocument["email"].type() == bsoncxx::type::k_string)
    {
        userDb.email = std::string(userDocument["email"].get_string().value);
    }

    if (userDocument["googleId"].type() == bsoncxx::type::k_string)
    {
        userDb.googleId = std::string(userDocument["googleId"].get_string().value);
    }

    if (userDocument["settings"].type() == bsoncxx::type::k_document)
    {
        auto doc = userDocument["settings"].get_document().value;

        userDb.settings.defaultCurrency = std::string(doc["defaultCurrency"].get_string().value);
    }
}

std::variant<UserDb, app::error::AppError> app::users::UsersRepositoryImpl::getUserByQuery(bsoncxx::document::value query)
{
    try
    {
        auto client = MongoAccess::instance().get_connection();

        auto db = (*client)["expenso-app"];

        stdx::optional<document::value> result = db.collection("users").find_one(query.view());

        if (!result.has_value())
        {
            return std::move(
                app::error::AppError{
                    .message = "Document not found",
                    .code = app::error::AppErrorCode::DB_NOT_FOUND});
        }

        UserDb userDb = {};

        fillUserDb(result.value(), userDb);

        return std::move(userDb);
    }
    catch (std::exception &exception)
    {
        return std::move(
            app::error::AppError{
                .message = exception.what(),
                .code = app::error::AppErrorCode::DB_QUERY_ERROR});
    }
}

std::variant<UserDb, app::error::AppError> app::users::UsersRepositoryImpl::createUserByQueryIfNotExist(bsoncxx::document::value query, const UserDb &user)
{
    try
    {
        auto client = MongoAccess::instance().get_connection();

        auto db = (*client)["expenso-app"];

        bsoncxx::document::value insertData = make_document(
            kvp("_id", user.id),
            kvp("firstName", user.firstName),
            kvp("alias", user.alias),
            kvp("email", user.email),
            kvp("googleId", user.googleId),
            kvp("lastName", user.lastName),

            kvp("settings", make_document(
                                kvp("defaultCurrency", user.settings.defaultCurrency))));

        mongocxx::options::find_one_and_update options;
        options.upsert(true);

        options.return_document(mongocxx::options::return_document::k_after);

        stdx::optional<bsoncxx::document::value> result =
            db
                .collection("users")
                .find_one_and_update(
                    query.view(),
                    make_document(kvp("$setOnInsert", insertData.view())),
                    options);

        if (!result.has_value())
        {
            return std::move(
                app::error::AppError{
                    .message = "Couldn't insert document",
                    .code = app::error::AppErrorCode::DB_INSERT_ERROR});
        }

        UserDb userDb = {};

        fillUserDb(result.value(), userDb);

        return std::move(userDb);
    }
    catch (std::exception &exception)
    {
        return std::move(
            app::error::AppError{
                .message = exception.what(),
                .code = app::error::AppErrorCode::DB_INSERT_ERROR});
    }
}
