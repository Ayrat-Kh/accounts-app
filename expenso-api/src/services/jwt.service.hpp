#pragma once

#include <string>
#include <string_view>
#include <optional>

#include "users/users.models.hpp"
#include "models.service.hpp"

namespace app
{
    namespace services
    {
        struct IJwtService
        {
            virtual std::string createUserToken(std::string_view userId) = 0;

            virtual std::optional<AuthUser> getAuthUser(std::string_view jwtToken) = 0;
        };

        class JwtServiceImpl : public IJwtService
        {
            std::string _jwtSecret;

        public:
            JwtServiceImpl(std::string _jwtSecret);

            virtual std::string createUserToken(std::string_view userId) override;

            virtual std::optional<AuthUser> getAuthUser(std::string_view jwtToken) override;
        };
    }
}
