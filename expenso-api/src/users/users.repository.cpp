#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "users.repository.hpp"
#include "utils/enumToString.hpp"
#include "utils/mongoDocument.hpp"

using namespace app::users;
using namespace app::services;
using namespace bsoncxx::v_noabi;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

UsersRepositoryImpl::UsersRepositoryImpl(std::shared_ptr<app::services::IMongoAccess> mongoAccess)
    : _mongoAccess(mongoAccess)
{
}

std::variant<UserDb, app::shared::AppError> UsersRepositoryImpl::getUserByGoogleId(std::string_view googleId)
{
    return std::move(
        getUserByQuery(
            std::move(
                make_document(kvp("googleId", googleId)))));
}

std::variant<UserDb, app::shared::AppError> UsersRepositoryImpl::getUserById(std::string_view userId)
{
    return std::move(
        getUserByQuery(
            std::move(
                make_document(kvp("_id", userId)))));
}

std::variant<UserDb, app::shared::AppError> UsersRepositoryImpl::createUserByIdIfNotExist(UserDb user)
{
    return std::move(
        createUserByQueryIfNotExist(
            std::move(
                make_document(
                    kvp("_id", user.id))),
            user));
}

std::variant<UserDb, app::shared::AppError> UsersRepositoryImpl::createUserByGoogleIdIfNotExist(UserDb user)
{
    return std::move(
        createUserByQueryIfNotExist(
            std::move(
                make_document(
                    kvp("googleId", user.googleId))),
            user));
}

std::variant<UserDb, app::shared::AppError> app::users::UsersRepositoryImpl::getUserByQuery(bsoncxx::document::value query)
{
    auto client = _mongoAccess->getConnection();
    auto db = (*client)["expenso-app"];

    try
    {
        stdx::optional<document::value> result = db.collection("users").find_one(query.view());

        if (!result.has_value())
        {
            return std::move(
                app::shared::AppError{
                    .message = "Document not found",
                    .code = app::utils::enumToString(app::shared::AppErrorCode::DB_NOT_FOUND)});
        }

        return std::move(app::utils::deserializeMongoDocument<UserDb>(result.value().view()));
    }
    catch (std::exception &exception)
    {
        return std::move(
            app::shared::AppError{
                .message = exception.what(),
                .code = app::utils::enumToString(app::shared::AppErrorCode::DB_QUERY_ERROR)});
    }
}

std::variant<UserDb, app::shared::AppError> app::users::UsersRepositoryImpl::createUserByQueryIfNotExist(bsoncxx::document::value query, const UserDb &user)
{
    auto client = _mongoAccess->getConnection();
    auto db = (*client)["expenso-app"];

    try
    {
        bsoncxx::builder::basic::document insertData = app::utils::toMongoDocument(std::move(user));

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
                app::shared::AppError{
                    .message = "Couldn't insert document",
                    .code = app::utils::enumToString(app::shared::AppErrorCode::DB_INSERT_ERROR)});
        }

        return std::move(app::utils::deserializeMongoDocument<UserDb>(result.value().view()));
    }
    catch (std::exception &exception)
    {
        return std::move(
            app::shared::AppError{
                .message = exception.what(),
                .code = app::utils::enumToString(app::shared::AppErrorCode::DB_INSERT_ERROR)});
    }
}
