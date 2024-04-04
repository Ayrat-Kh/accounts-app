#include <jwt-cpp/jwt.h>

#include "jwt.service.hpp"

app::services::JwtServiceImpl::JwtServiceImpl(std::string_view jwtSecret)
    : _jwtSecret(jwtSecret)
{
}

std::string app::services::JwtServiceImpl::createUserToken(std::string_view userId)
{
    return std::move(
        jwt::create()
            .set_type("JWS")
            .set_issuer("auth0")
            .set_payload_claim("user_id", jwt::claim(std::string(userId)))
            .sign(jwt::algorithm::hs256{_jwtSecret}));
}

bool app::services::JwtServiceImpl::verify(std::string_view token)
{
    return false;
}
