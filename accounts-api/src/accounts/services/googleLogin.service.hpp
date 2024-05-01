#pragma once

#include <string>
#include <string_view>

#include <boost/asio/awaitable.hpp>
#include <boost/describe.hpp>

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
    BOOST_DESCRIBE_STRUCT(GoogleTokenInfo, (), (alg, at_hash, aud, azp, email, email_verified, exp, family_name, given_name, iat, iss, kid, name, picture, sub, typ))

    class IGoogleLoginService
    {
    public:
        virtual boost::asio::awaitable<AccountsResult<GoogleTokenInfo>> getGoogleUser(std::string_view idToken) = 0;
    };

    class GoogleLoginServiceImpl : public IGoogleLoginService
    {
    public:
        virtual boost::asio::awaitable<AccountsResult<GoogleTokenInfo>> getGoogleUser(std::string_view idToken) noexcept override;
    };
}
