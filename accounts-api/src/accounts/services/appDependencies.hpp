#pragma once

#include <boost/asio/io_context.hpp>

#include "accounts/auth/auth.service.hpp"
#include "accounts/users/users.repository.hpp"
#include "accounts/accounts/accounts.repository.hpp"
#include "accounts/services/mongoAccess.hpp"
#include "accounts/services/googleLogin.service.hpp"
#include "accounts/services/geoLocReverse.service.hpp"
#include "accounts/services/jwt.service.hpp"

namespace accounts
{
    struct AppDependencies
    {
        // dependency section
        std::shared_ptr<IGoogleLoginService>
            googleLoginService = nullptr;
        std::shared_ptr<IJwtService> jwtService = nullptr;
        std::shared_ptr<IUsersRepository> userRepo = nullptr;
        std::shared_ptr<IAccountsRepository> accountsRepo = nullptr;
        std::shared_ptr<IAuthService> authService = nullptr;
        std::shared_ptr<IMongoAccess> mongoAccess = nullptr;
        std::shared_ptr<IGeoLocReverseService> geoLocReverseService = nullptr;

        boost::asio::io_context ioContext;

        static AppDependencies &instance();

        void init();

    private:
        AppDependencies() = default;
    };

}