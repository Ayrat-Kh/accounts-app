#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "users.repository.hpp"
#include "accounts/shared/env.hpp"
#include "accounts/utils/enumHelpers.hpp"
#include "accounts/utils/mongoDocument.hpp"

using namespace accounts;

using namespace bsoncxx::v_noabi;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

UsersRepositoryImpl::UsersRepositoryImpl(std::shared_ptr<IMongoAccess> mongoAccess)
    : _mongoAccess(mongoAccess)
{
}

std::variant<UserDb, AppError> UsersRepositoryImpl::getUserByGoogleId(std::string_view googleId)
{
    return std::move(
        getUserByQuery(
            std::move(
                make_document(kvp("googleId", googleId)))));
}

std::variant<UserDb, AppError> UsersRepositoryImpl::getUserById(std::string_view userId)
{
    return std::move(
        getUserByQuery(
            std::move(
                make_document(kvp("_id", userId)))));
}

std::variant<UserDb, AppError> UsersRepositoryImpl::upsertUserByIdIfNotExist(std::string_view userId, UpsertUserDb user)
{
    auto client = _mongoAccess->getConnection();
    auto db = (*client)[accounts::getEnv().dbName];

    try
    {
        auto root = make_document(
            kvp("$set", std::move(toMongoDocument(std::move(user)))),
            kvp("$setOnInsert", make_document(kvp("createdAt", bsoncxx::types::b_date(std::chrono::system_clock::now())))));

        mongocxx::options::find_one_and_update options;
        options.upsert(true);
        options.return_document(mongocxx::options::return_document::k_after);

        stdx::optional<bsoncxx::document::value> result =
            db
                .collection(collectionName)
                .find_one_and_update(
                    make_document(
                        kvp("_id", userId)),
                    std::move(root),
                    options);

        if (!result.has_value())
        {
            return std::move(
                AppError{
                    .code = enumToString(EAppErrorCode::DB_INSERT_ERROR),
                    .message = "Couldn't insert document"});
        }

        return std::move(deserializeMongoDocument<UserDb>(result.value().view()));
    }
    catch (std::exception &exception)
    {
        return std::move(
            AppError{
                .code = enumToString(EAppErrorCode::DB_INSERT_ERROR),
                .message = exception.what()});
    }
}

std::variant<UserDb, AppError> UsersRepositoryImpl::createUserByGoogleIdIfNotExist(GoogleUpsertUserDb user)
{
    auto client = _mongoAccess->getConnection();
    auto db = (*client)[accounts::getEnv().dbName];

    auto userDocument = toMongoDocument(std::move(user));
    userDocument.append(kvp("createdAt", bsoncxx::types::b_date(std::chrono::system_clock::now())));

    try
    {
        auto root = make_document(
            kvp("$setOnInsert", std::move(userDocument)));

        mongocxx::options::find_one_and_update options;
        options.upsert(true);
        options.return_document(mongocxx::options::return_document::k_after);

        stdx::optional<bsoncxx::document::value> result =
            db
                .collection(collectionName)
                .find_one_and_update(
                    make_document(
                        kvp("googleId", user.googleId)),
                    std::move(root),
                    options);

        if (!result.has_value())
        {
            return std::move(
                AppError{
                    .code = enumToString(EAppErrorCode::DB_INSERT_ERROR),
                    .message = "Couldn't insert document"});
        }

        return std::move(deserializeMongoDocument<UserDb>(result.value().view()));
    }
    catch (std::exception &exception)
    {
        return std::move(
            AppError{
                .code = enumToString(EAppErrorCode::DB_INSERT_ERROR),
                .message = exception.what()});
    }
}

std::variant<UserDb, AppError> UsersRepositoryImpl::getUserByQuery(bsoncxx::document::value query)
{
    auto client = _mongoAccess->getConnection();
    auto db = (*client)[accounts::getEnv().dbName];

    try
    {
        stdx::optional<document::value> result = db.collection(collectionName).find_one(query.view());

        if (!result.has_value())
        {
            return std::move(
                AppError{
                    .code = enumToString(EAppErrorCode::DB_NOT_FOUND),
                    .message = "Document not found"});
        }

        return std::move(deserializeMongoDocument<UserDb>(result.value().view()));
    }
    catch (std::exception &exception)
    {
        return std::move(
            AppError{
                .code = enumToString(EAppErrorCode::DB_QUERY_ERROR),
                .message = exception.what()});
    }
}
