#include "user.repository.hpp"
#include "services/mongo-access.hpp"

#include <mongocxx/client.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

using namespace app::user;
using namespace app::service;
using namespace bsoncxx::v_noabi;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

std::variant<UserDb, std::exception_ptr> UserRepositoryImpl::GetUserById(std::string_view userId)
{
    try
    {
        auto client = app::service::MongoAccess::instance().get_connection();

        auto db = (*client)["expenso-app"];

        stdx::optional<document::value> result = db.collection("users").find_one(make_document(kvp("_id", userId)));

        if (!result.has_value())
        {
            throw std::domain_error("Not found");
        }

        bsoncxx::document::value &v = result.value();

        UserDb userDb = {};

        if (v["firstName"].type() == bsoncxx::type::k_string)
        {
            userDb.firstName = std::string(v["firstName"].get_string().value);
        }

        if (v["lastName"].type() == bsoncxx::type::k_string)
        {
            userDb.lastName = std::string(v["lastName"].get_string().value);
        }

        if (v["email"].type() == bsoncxx::type::k_string)
        {
            userDb.email = std::string(v["email"].get_string().value);
        }

        if (v["googleId"].type() == bsoncxx::type::k_string)
        {
            userDb.googleId = std::string(v["googleId"].get_string().value);
        }

        if (v["settings"].type() == bsoncxx::type::k_document)
        {
            auto doc = v["settings"].get_document().value;

            userDb.settings.defaultCurrency = std::string(doc["defaultCurrency"].get_string().value);
        }

        return userDb;
    }
    catch (std::exception &exception)
    {
        return std::current_exception();
    }
}

std::variant<std::string_view, std::exception_ptr> UserRepositoryImpl::UpsertUserById(UserDb user)
{
    try
    {
        auto client = app::service::MongoAccess::instance().get_connection();

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

        stdx::optional<mongocxx::result::insert_one> result = db.collection("users").insert_one(insertData.view());

        if (!result.has_value())
        {
            throw std::domain_error("Couldn't insert document");
        }

        if (result->inserted_id().type() == bsoncxx::type::k_oid)
        {
            bsoncxx::oid id = result->inserted_id().get_oid().value;
            return id.to_string();
        }
        else
        {
            throw std::domain_error("Inserted id was not an OID type");
        }

        return std::string(result.value().inserted_id().get_string().value);
    }
    catch (std::exception &exception)
    {
        return std::current_exception();
    }
}
