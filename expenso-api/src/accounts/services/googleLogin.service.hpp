#pragma once

#include <variant>
#include <string>
#include <string_view>

#include "accounts/shared/models.hpp"

namespace accounts::services
{
    struct GoogleTokenInfo
    {
        std::string iss;
        std::string azp;
        std::string aud;
        std::string sub;
        std::string exp;
        std::string email;
        std::string email_verified;
        std::string at_hash;
        std::string given_name;
        std::string family_name;
        std::string name;
        std::string picture;
        std::string iat;
        std::string alg;
        std::string kid;
        std::string typ;
    };

    class IGoogleLoginService
    {
    public:
        virtual std::variant<GoogleTokenInfo, shared::AppError> getGoogleUser(std::string_view idToken) = 0;
    };

    class GoogleLoginServiceImpl : public IGoogleLoginService
    {
    public:
        virtual std::variant<GoogleTokenInfo, shared::AppError> getGoogleUser(std::string_view idToken) noexcept override;
    };
}