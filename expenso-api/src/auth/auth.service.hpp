#pragma once

#include <variant>

#include "auth/auth.models.hpp"
#include "services/googleLogin.service.hpp"
#include "users/users.repository.hpp"
#include "shared/models.hpp"
#include "services/jwt.service.hpp"

namespace app::auth
{
    class IAuthService
    {
    public:
        virtual std::variant<UserLoginResult, app::shared::AppError> googleAuth(std::string_view accessToken) = 0;
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

        virtual std::variant<UserLoginResult, app::shared::AppError> googleAuth(std::string_view idToken);
    };
}