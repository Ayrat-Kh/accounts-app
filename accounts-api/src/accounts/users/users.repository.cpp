#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "users.repository.hpp"
#include "accounts/utils/enumToString.hpp"
#include "accounts/utils/mongoDocument.hpp"

using namespace ::accounts::services;
using namespace ::accounts::shared;
using namespace ::accounts::utils;

using namespace bsoncxx::v_noabi;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

accounts::users::UsersRepositoryImpl::UsersRepositoryImpl(std::shared_ptr<IMongoAccess> mongoAccess)
    : _mongoAccess(mongoAccess)
{
}

std::variant<UserDb, AppError> accounts::users::UsersRepositoryImpl::getUserByGoogleId(std::string_view googleId)
{
    return std::move(
        getUserByQuery(
            std::move(
                make_document(kvp("googleId", googleId)))));
}

std::variant<UserDb, AppError> accounts::users::UsersRepositoryImpl::getUserById(std::string_view userId)
{
    return std::move(
        getUserByQuery(
            std::move(
                make_document(kvp("_id", userId)))));
}

std::variant<UserDb, AppError> accounts::users::UsersRepositoryImpl::createUserByIdIfNotExist(UserDb user)
{
    return std::move(
        createUserByQueryIfNotExist(
            std::move(
                make_document(
                    kvp("_id", user.id))),
            user));
}

std::variant<UserDb, AppError> accounts::users::UsersRepositoryImpl::createUserByGoogleIdIfNotExist(UserDb user)
{
    return std::move(
        createUserByQueryIfNotExist(
            std::move(
                make_document(
                    kvp("googleId", user.googleId))),
            user));
}

std::variant<UserDb, AppError> accounts::users::UsersRepositoryImpl::getUserByQuery(bsoncxx::document::value query)
{
    auto client = _mongoAccess->getConnection();
    auto db = (*client)["expenso-app"];

    try
    {
        stdx::optional<document::value> result = db.collection("users").find_one(query.view());

        if (!result.has_value())
        {
            return std::move(
                AppError{
                    .code = enumToString(AppErrorCode::DB_NOT_FOUND),
                    .message = "Document not found"});
        }

        return std::move(deserializeMongoDocument<UserDb>(result.value().view()));
    }
    catch (std::exception &exception)
    {
        return std::move(
            AppError{
                .code = enumToString(AppErrorCode::DB_QUERY_ERROR),
                .message = exception.what()});
    }
}

std::variant<UserDb, AppError> accounts::users::UsersRepositoryImpl::createUserByQueryIfNotExist(bsoncxx::document::value query, const UserDb &user)
{
    auto client = _mongoAccess->getConnection();
    auto db = (*client)["expenso-app"];

    try
    {
        bsoncxx::builder::basic::document insertData = toMongoDocument(std::move(user));

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
                AppError{
                    .code = enumToString(AppErrorCode::DB_INSERT_ERROR),
                    .message = "Couldn't insert document"});
        }

        return std::move(deserializeMongoDocument<UserDb>(result.value().view()));
    }
    catch (std::exception &exception)
    {
        return std::move(
            AppError{
                .code = enumToString(AppErrorCode::DB_INSERT_ERROR),
                .message = exception.what()});
    }
}
