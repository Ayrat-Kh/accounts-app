#pragma once

#include <boost/asio.hpp>

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
        virtual boost::asio::awaitable<AccountsResult<UserLoginResult>> googleAuth(std::string_view accessToken) = 0;
    };

    class AuthServiceImpl : public IAuthService
    {
        std::shared_ptr<IGoogleLoginService> _googleLoginService;
        std::shared_ptr<IUsersRepository> _userRepository;
        std::shared_ptr<IJwtService> _jwtService;

    public:
        AuthServiceImpl(std::shared_ptr<IGoogleLoginService> _googleLoginService,
                        std::shared_ptr<IUsersRepository> _userRepository,
                        std::shared_ptr<IJwtService> _jwtService);

        virtual boost::asio::awaitable<AccountsResult<UserLoginResult>> googleAuth(std::string_view idToken);
    };
}