// accounts-api: stores receipt prices and photos for the Android app.
//
// This file is the composition root: the only place that knows concrete
// classes. Everything else receives its dependencies through constructors,
// so any piece can be replaced (e.g. by a fake in tests) without touching
// the code that uses it.
//
// Threading: one io_context, run on one thread. uWebSockets, the Postgres
// pool and every request coroutine share it, so there are no locks. The only
// other threads belong to BlockingExecutor, for the AWS SDK's blocking calls.

#include "assets/AssetRoutes.hpp"
#include "auth/AccessTokenService.hpp"
#include "auth/AuthRoutes.hpp"
#include "auth/AuthService.hpp"
#include "auth/GoogleIdentityVerifier.hpp"
#include "auth/RefreshTokenRepository.hpp"
#include "auth/UserRepository.hpp"
#include "config/Config.hpp"
#include "db/Migrator.hpp"
#include "db/PgPool.hpp"
#include "health/HealthRoutes.hpp"
#include "http/Router.hpp"
#include "http/Server.hpp"
#include "net/HttpsClient.hpp"
#include "receipts/ReceiptRepository.hpp"
#include "receipts/ReceiptRoutes.hpp"
#include "receipts/ReceiptService.hpp"
#include "storage/S3ObjectStore.hpp"
#include "util/BlockingExecutor.hpp"

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>

#include <csignal>
#include <string_view>

namespace asio = boost::asio;
using namespace accounts;

namespace {

constexpr std::string_view defaultConfigPath = "/etc/accounts/accounts.toml";
constexpr std::size_t blockingThreadCount = 4;

void configureLogging() {
    spdlog::set_default_logger(spdlog::stderr_logger_mt("accounts"));
    spdlog::set_pattern("%Y-%m-%dT%H:%M:%S.%eZ %l %v", spdlog::pattern_time_type::utc);
}

std::string configPathFromArguments(int argc, char **argv) {
    for (int i = 1; i + 1 < argc; ++i) {
        if (std::string_view(argv[i]) == "--config") {
            return argv[i + 1];
        }
    }
    return std::string(defaultConfigPath);
}

// Runs one coroutine to completion before the server starts (used for migrations).
Result<void> runToCompletion(asio::io_context &ioContext, asio::awaitable<Result<void>> task) {
    std::optional<Result<void>> outcome;
    asio::co_spawn(ioContext, std::move(task), [&outcome](const std::exception_ptr &error, Result<void> result) {
        if (error) {
            std::rethrow_exception(error);
        }
        outcome = std::move(result);
    });
    ioContext.run();
    ioContext.restart();
    return *outcome;
}

void serve(const Config &config) {
    asio::io_context ioContext;

    if (auto migrated =
            runToCompletion(ioContext, db::runMigrations(config.database.connectionString, db::embeddedMigrations()));
        !migrated) {
        throw std::runtime_error(migrated.error().message);
    }

    // Infrastructure
    storage::AwsSdkScope awsSdk;
    BlockingExecutor blocking(blockingThreadCount);
    db::PgPool pool(config.database.connectionString, config.database.poolSize);
    storage::S3ObjectStore objectStore(config.s3, blocking);
    net::HttpsClient httpsClient;

    // Authentication
    auth::AccessTokenService accessTokens(config.auth.jwtPrivateKeyPem, config.auth.jwtIssuer,
                                          config.auth.accessTokenTtl);
    auth::GoogleIdentityVerifier googleIdentity(httpsClient, config.auth.googleClientIds);
    auth::UserRepository users(pool);
    auth::RefreshTokenRepository refreshTokens(pool);
    auth::AuthService authService(googleIdentity, users, refreshTokens, accessTokens,
                                  {
                                      .allowedEmails = config.auth.allowedEmails,
                                      .refreshTokenTtl = config.auth.refreshTokenTtl,
                                      .refreshTokenAbsoluteTtl = config.auth.refreshTokenAbsoluteTtl,
                                  });

    // Receipts
    receipts::ReceiptRepository receiptRepository(pool);
    receipts::ReceiptService receiptService(receiptRepository, objectStore);

    // HTTP
    http::Router router(accessTokens, config.server.maxJsonBodyBytes);
    health::registerHealthRoutes(router, pool);
    auth::registerAuthRoutes(router, authService);
    receipts::registerReceiptRoutes(router, receiptService, config.server.maxImageBodyBytes);
    assets::registerAssetRoutes(router, objectStore);

    http::Server server(ioContext, router);
    server.listen(config.server.bindHost, config.server.port);
    spdlog::info("listening on {}:{}", config.server.bindHost, server.port());

    // Kubernetes sends SIGTERM before killing the pod: stop accepting, let
    // in-flight requests finish, and exit once connections are closed.
    asio::signal_set signals(ioContext, SIGINT, SIGTERM);
    signals.async_wait([&server](const boost::system::error_code &, int signal) {
        spdlog::info("signal {} received, shutting down", signal);
        server.stop();
    });

    server.run();
    spdlog::info("stopped");
}

} // namespace

int main(int argc, char **argv) {
    configureLogging();
    try {
        serve(loadConfig(configPathFromArguments(argc, argv)));
        return 0;
    } catch (const std::exception &error) {
        spdlog::critical("fatal: {}", error.what());
        return 1;
    }
}
