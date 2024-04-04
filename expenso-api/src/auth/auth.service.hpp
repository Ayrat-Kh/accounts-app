#pragma once

#include <variant>

#include "auth/auth.models.hpp"
#include "services/google-login.service.hpp"
#include "users/users.repository.hpp"
#include "utils/error.hpp"
#include "services/jwt.service.hpp"

namespace app
{
    namespace auth
    {
        class IAuthService
        {
        public:
            virtual std::variant<UserLoginResult, app::error::AppError> googleAuth(std::string_view accessToken) = 0;
        };

        class AuthServiceImpl : public IAuthService
        {
            std::shared_ptr<app::services::IGoogleLoginService> _googleLoginService;
            std::shared_ptr<app::users::IUsersRepository> _userRepository;
            std::shared_ptr<app::services::IJwtService> _jwtService;

        public:
            AuthServiceImpl(std::shared_ptr<app::services::IGoogleLoginService> googleLoginService,
                            std::shared_ptr<app::users::IUsersRepository> userRepository,
                            std::shared_ptr<app::services::IJwtService> _jwtService);

            virtual std::variant<UserLoginResult, app::error::AppError> googleAuth(std::string_view idToken);
        };
    }
}