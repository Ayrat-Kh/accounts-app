#pragma once

#include <variant>

#include "accounts/auth/auth.models.hpp"
#include "accounts/users/users.repository.hpp"
#include "accounts/shared/models.hpp"
#include "accounts/services/googleLogin.service.hpp"
#include "accounts/services/jwt.service.hpp"

namespace accounts::auth
{
    class IAuthService
    {
    public:
        virtual std::variant<UserLoginResult, shared::AppError> googleAuth(std::string_view accessToken) = 0;
    };

    class AuthServiceImpl : public IAuthService
    {
        std::shared_ptr<services::IGoogleLoginService> _googleLoginService;
        std::shared_ptr<users::IUsersRepository> _userRepository;
        std::shared_ptr<services::IJwtService> _jwtService;

    public:
        AuthServiceImpl(std::shared_ptr<services::IGoogleLoginService> googleLoginService,
                        std::shared_ptr<users::IUsersRepository> userRepository,
                        std::shared_ptr<services::IJwtService> _jwtService);

        virtual std::variant<UserLoginResult, shared::AppError> googleAuth(std::string_view idToken);
    };
}