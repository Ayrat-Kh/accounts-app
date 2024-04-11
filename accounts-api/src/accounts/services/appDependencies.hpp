#pragma once

#include "accounts/auth/auth.service.hpp"
#include "accounts/users/users.repository.hpp"
#include "accounts/accounts/accounts.repository.hpp"
#include "accounts/services/mongoAccess.hpp"
#include "accounts/services/googleLogin.service.hpp"
#include "accounts/services/jwt.service.hpp"

namespace accounts::services
{
    struct AppDependencies
    {
        // dependency section
        std::shared_ptr<IGoogleLoginService>
            googleLoginService = nullptr;
        std::shared_ptr<IJwtService> jwtService = nullptr;
        std::shared_ptr<users::IUsersRepository> userRepo = nullptr;
        std::shared_ptr<accounts::IAccountsRepository> accountsRepo = nullptr;
        std::shared_ptr<auth::IAuthService> authService = nullptr;
        std::shared_ptr<services::IMongoAccess> mongoAccess = nullptr;

        static AppDependencies &instance();

        void init();

    private:
        AppDependencies() = default;
    };

}