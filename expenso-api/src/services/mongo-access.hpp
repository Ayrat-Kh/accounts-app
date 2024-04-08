#pragma once

#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/client.hpp>

namespace app::services
{
    struct IMongoAccess
    {
        using connection = mongocxx::pool::entry;

        virtual connection getConnection() = 0;

        virtual bsoncxx::stdx::optional<connection> tryGetConnection() = 0;
    };

    class MongoAccessImpl : public IMongoAccess
    {
    public:
        void configure(
            std::unique_ptr<mongocxx::instance> instance,
            std::unique_ptr<mongocxx::pool> pool);

        virtual connection getConnection() override;

        virtual bsoncxx::stdx::optional<connection> tryGetConnection() override;

    private:
        std::unique_ptr<mongocxx::instance> _instance = nullptr;
        std::unique_ptr<mongocxx::pool> _pool = nullptr;
    };

    void configureMongoInstance(mongocxx::uri uri, std::shared_ptr<MongoAccessImpl>);

}
