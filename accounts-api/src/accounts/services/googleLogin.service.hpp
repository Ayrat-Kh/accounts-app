#pragma once

#include <variant>
#include <string>
#include <string_view>

#include "accounts/shared/models.hpp"

namespace accounts
{
    struct GoogleTokenInfo
    {
        std::string alg;
        std::string at_hash;
        std::string aud;
        std::string azp;
        std::string email;
        std::string email_verified;
        std::string exp;
        std::string family_name;
        std::string given_name;
        std::string iat;
        std::string iss;
        std::string kid;
        std::string name;
        std::string picture;
        std::string sub;
        std::string typ;
    };

    class IGoogleLoginService
    {
    public:
        virtual std::variant<GoogleTokenInfo, AppError> getGoogleUser(std::string_view idToken) = 0;
    };

    class GoogleLoginServiceImpl : public IGoogleLoginService
    {
    public:
        virtual std::variant<GoogleTokenInfo, AppError> getGoogleUser(std::string_view idToken) noexcept override;
    };
}
