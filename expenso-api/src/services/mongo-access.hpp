#pragma once

#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>

namespace app
{
    namespace service
    {
        class MongoAccess
        {
        public:
            static MongoAccess &instance()
            {
                static MongoAccess instance;
                return instance;
            }

            void Configure(
                std::unique_ptr<mongocxx::instance> instance,
                std::unique_ptr<mongocxx::pool> pool);

            using connection = mongocxx::pool::entry;

            connection get_connection()
            {
                return _pool->acquire();
            }

            bsoncxx::stdx::optional<connection> try_get_connection()
            {
                return _pool->try_acquire();
            }

        private:
            MongoAccess() = default;

            std::unique_ptr<mongocxx::instance> _instance = nullptr;
            std::unique_ptr<mongocxx::pool> _pool = nullptr;
        };

        void ConfigureMongoInstance(mongocxx::uri uri);
    }
}
