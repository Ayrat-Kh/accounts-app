#pragma once

#include "auth/auth.service.hpp"
#include "users/users.repository.hpp"
#include "accounts/accounts.repository.hpp"
#include "services/mongo-access.hpp"
#include "services/google-login.service.hpp"
#include "services/jwt.service.hpp"

namespace app::services
{
    struct AppDependencies
    {
        // dependency section
        std::shared_ptr<app::services::IGoogleLoginService> googleLoginService = nullptr;
        std::shared_ptr<app::services::IJwtService> jwtService = nullptr;
        std::shared_ptr<app::users::IUsersRepository> userRepo = nullptr;
        std::shared_ptr<app::accounts::IAccountsRepository> accountsRepo = nullptr;
        std::shared_ptr<app::auth::IAuthService> authService = nullptr;
        std::shared_ptr<app::services::IMongoAccess> mongoAccess = nullptr;

        static AppDependencies &instance();

        void init();

    private:
        AppDependencies() = default;
    };

}