#include "mongo-access.hpp"

#include <mongocxx/logger.hpp>
#include <mongocxx/stdx.hpp>
#include <bsoncxx/stdx/make_unique.hpp>

// mongocxx::instance instance = {}; // This should be done only once.
// mongocxx::uri uri("mongodb://localhost:27017");
// mongocxx::client client(uri);

void app::service::MongoAccess::Configure(std::unique_ptr<mongocxx::instance> instance,
                                          std::unique_ptr<mongocxx::pool> pool)
{
    _instance = std::move(instance);
    _pool = std::move(pool);
}

void app::service::ConfigureMongoInstance(mongocxx::uri uri)
{
    class noop_logger : public mongocxx::logger
    {
    public:
        virtual void operator()(
            mongocxx::log_level,
            bsoncxx::stdx::string_view,
            bsoncxx::stdx::string_view) noexcept {}
    };

    auto instance =
        bsoncxx::stdx::make_unique<mongocxx::instance>(bsoncxx::stdx::make_unique<noop_logger>());

    MongoAccess::instance().Configure(
        std::move(instance),
        bsoncxx::stdx::make_unique<mongocxx::pool>(std::move(uri)));
}