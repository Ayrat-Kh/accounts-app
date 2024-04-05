#include "app-dependencies.hpp"

app::services::AppDependencies &app::services::AppDependencies::instance()
{
    static app::services::AppDependencies instance = {};
    return instance;
}

void app::services::AppDependencies::init()
{
    std::string secretKey = "secret super duper key";

    std::shared_ptr<app::services::MongoAccessImpl> mongoAccess = std::make_shared<app::services::MongoAccessImpl>();
    app::services::configureMongoInstance(mongocxx::uri("mongodb://localhost:27017"), mongoAccess);
    this->mongoAccess = mongoAccess;

    googleLoginService = std::make_shared<app::services::GoogleLoginServiceImpl>();
    jwtService = std::make_shared<app::services::JwtServiceImpl>(secretKey);
    userRepo = std::make_shared<app::users::UsersRepositoryImpl>(mongoAccess);
    authService = std::make_shared<app::auth::AuthServiceImpl>(googleLoginService, userRepo, jwtService);
}
