#include "appDependencies.hpp"

using namespace ::accounts::users;
using namespace ::accounts::accounts;
using namespace ::accounts::auth;

accounts::services::AppDependencies &accounts::services::AppDependencies::instance()
{
    static AppDependencies instance = {};
    return instance;
}

void accounts::services::AppDependencies::init()
{
    std::string secretKey = "secret super duper key";

    std::shared_ptr<MongoAccessImpl> mongoAccess = std::make_shared<MongoAccessImpl>();
    configureMongoInstance(mongocxx::uri("mongodb://localhost:27017"), mongoAccess);
    this->mongoAccess = mongoAccess;

    googleLoginService = std::make_shared<GoogleLoginServiceImpl>();
    jwtService = std::make_shared<JwtServiceImpl>(secretKey);
    userRepo = std::make_shared<UsersRepositoryImpl>(mongoAccess);
    accountsRepo = std::make_shared<AccountsRepositoryImpl>(mongoAccess);
    authService = std::make_shared<AuthServiceImpl>(googleLoginService, userRepo, jwtService);
}
