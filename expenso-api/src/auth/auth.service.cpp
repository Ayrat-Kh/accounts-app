#include <exception>

#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "auth.service.hpp"
#include "utils/error.hpp"

app::auth::AuthServiceImpl::AuthServiceImpl(std::shared_ptr<app::services::IGoogleLoginService> googleLoginService,
                                            std::shared_ptr<app::users::IUsersRepository> userRepository,
                                            std::shared_ptr<app::services::IJwtService> jwtService)
    : _googleLoginService(googleLoginService),
      _userRepository(userRepository), _jwtService(jwtService)
{
}

std::variant<app::auth::UserLoginResult, app::error::AppError> app::auth::AuthServiceImpl::googleAuth(std::string_view idToken)
{
    auto googleAuthResult = _googleLoginService.get()->getGoogleUser(idToken);

    if (const app::error::AppError *error = app::error::isError(&googleAuthResult))
    {
        return std::move(*error);
    }

    app::services::GoogleTokenInfo googleLoginResult = std::get<app::services::GoogleTokenInfo>(std::move(googleAuthResult));

    auto userDbResult = _userRepository.get()->createUserByGoogleIdIfNotExist(app::users::UserDb{
        .id = boost::uuids::to_string(boost::uuids::random_generator()()),
        .googleId = std::move(googleLoginResult.sub),
        .email = std::move(googleLoginResult.email),
        .firstName = std::move(googleLoginResult.given_name),
        .alias = std::move(googleLoginResult.name),
        .lastName = std::move(googleLoginResult.given_name),
    });

    if (const app::error::AppError *error = app::error::isError(&userDbResult))
    {
        return std::move(*error);
    }

    auto user = std::get<app::users::UserDb>(std::move(userDbResult));

    auto accessToken = _jwtService.get()->createUserToken(user.id);

    return std::move(app::auth::UserLoginResult{
        .accessToken = std::move(accessToken),
        .sessionToken = "",
        .user = std::move(user)});
}
