#pragma once

#include <string_view>

#include "users/users.models.hpp"

namespace app
{
    namespace services
    {
        struct IJwtService
        {
            virtual std::string createUserToken(std::string_view userId) = 0;

            virtual bool verify(std::string_view token) = 0;
        };

        class JwtServiceImpl : public IJwtService
        {
            std::string _jwtSecret;

        public:
            JwtServiceImpl(std::string_view _jwtSecret);

            virtual std::string createUserToken(std::string_view userId) override;

            virtual bool verify(std::string_view token) override;
        };
    }
}
