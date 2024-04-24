#pragma once

#include <variant>

#include "accounts/auth/auth.models.hpp"
#include "accounts/users/users.repository.hpp"
#include "accounts/shared/models.hpp"
#include "accounts/services/googleLogin.service.hpp"
#include "accounts/services/jwt.service.hpp"

namespace accounts
{
    class IAuthService
    {
    public:
        virtual std::variant<UserLoginResult, AppError> googleAuth(std::string_view accessToken) = 0;
    };

    class AuthServiceImpl : public IAuthService
    {
        std::shared_ptr<IGoogleLoginService> _googleLoginService;
        std::shared_ptr<IUsersRepository> _userRepository;
        std::shared_ptr<IJwtService> _jwtService;

    public:
        AuthServiceImpl(std::shared_ptr<IGoogleLoginService> googleLoginService,
                        std::shared_ptr<IUsersRepository> userRepository,
                        std::shared_ptr<IJwtService> _jwtService);

        virtual std::variant<UserLoginResult, AppError> googleAuth(std::string_view idToken);
    };
}