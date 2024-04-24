#include <chrono>

#include <jwt-cpp/jwt.h>

#include "jwt.service.hpp"

using namespace accounts;

JwtServiceImpl::JwtServiceImpl(std::string jwtSecret)
    : _jwtSecret(std::move(jwtSecret))
{
}

std::string JwtServiceImpl::createUserToken(std::string_view userId)
{
    return std::move(
        jwt::create()
            .set_type("JWT")
            .set_issuer("accounts")
            .set_payload_claim("user_id", jwt::claim(std::string(userId)))
            .set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds{7 * 24 * 60 * 60})
            .sign(jwt::algorithm::hs256{_jwtSecret}));
}

std::optional<AuthUser> JwtServiceImpl::getAuthUser(std::string_view jwtToken)
{
    auto verifier =
        jwt::verify()
            .with_type("JWT")
            .with_issuer("accounts")
            .allow_algorithm(jwt::algorithm::hs256{_jwtSecret});

    try
    {
        jwtToken.remove_prefix(7);                              // remove "Bearer "
        auto decodedToken = jwt::decode(std::string(jwtToken)); // bad

        verifier.verify(decodedToken);

        return std::move(AuthUser{
            .userId = decodedToken.get_payload_claim("user_id").as_string()});
    }
    catch (std::exception &ex)
    {
        std::cout << "jwtToken" << ex.what() << std::endl;
        return {};
    }
}
