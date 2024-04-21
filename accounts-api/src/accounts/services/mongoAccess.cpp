#include "mongoAccess.hpp"

#include <mongocxx/logger.hpp>
#include <mongocxx/stdx.hpp>
#include <bsoncxx/stdx/make_unique.hpp>

void accounts::MongoAccessImpl::configure(std::unique_ptr<mongocxx::instance> instance,
                                          std::unique_ptr<mongocxx::pool> pool)
{
    _instance = std::move(instance);
    _pool = std::move(pool);
}

accounts::MongoAccessImpl::connection accounts::MongoAccessImpl::getConnection()
{
    return _pool->acquire();
}

bsoncxx::stdx::optional<accounts::MongoAccessImpl::connection> accounts::MongoAccessImpl::tryGetConnection()
{
    return _pool->try_acquire();
}

void accounts::configureMongoInstance(mongocxx::uri uri, std::shared_ptr<MongoAccessImpl> mongoAccess)
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

    mongoAccess->configure(
        std::move(instance),
        bsoncxx::stdx::make_unique<mongocxx::pool>(std::move(uri)));
}