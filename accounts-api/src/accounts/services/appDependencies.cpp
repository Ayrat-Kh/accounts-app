#include "appDependencies.hpp"
#include "accounts/shared/env.hpp"

accounts::AppDependencies &accounts::AppDependencies::instance()
{
    static AppDependencies instance = {};
    return instance;
}

void accounts::AppDependencies::init()
{
    auto &env = accounts::getEnv();

    std::shared_ptr<MongoAccessImpl> mongoAccess = std::make_shared<MongoAccessImpl>();
    configureMongoInstance(mongocxx::uri(env.mongoUrl), mongoAccess);
    this->mongoAccess = mongoAccess;

    geoLocReverseService =
        std::make_shared<GeoLocReverseServiceImpl>(
            std::move(env.geoCodeApiKey));
    googleLoginService = std::make_shared<GoogleLoginServiceImpl>();
    jwtService = std::make_shared<JwtServiceImpl>(env.jwtKey);
    userRepo = std::make_shared<UsersRepositoryImpl>(mongoAccess);
    accountsRepo = std::make_shared<AccountsRepositoryImpl>(mongoAccess);
    authService = std::make_shared<AuthServiceImpl>(googleLoginService, userRepo, jwtService);
}
