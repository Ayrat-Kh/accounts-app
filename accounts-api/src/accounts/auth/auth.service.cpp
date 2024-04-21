#include <exception>

#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "auth.service.hpp"
#include "accounts/utils/error.hpp"

using namespace ::accounts;

AuthServiceImpl::AuthServiceImpl(std::shared_ptr<IGoogleLoginService> googleLoginService,
                                 std::shared_ptr<IUsersRepository> userRepository,
                                 std::shared_ptr<IJwtService> jwtService)
    : _googleLoginService(googleLoginService),
      _userRepository(userRepository), _jwtService(jwtService)
{
}

std::variant<UserLoginResult, AppError> AuthServiceImpl::googleAuth(std::string_view idToken)
{
    auto googleAuthResult = _googleLoginService.get()->getGoogleUser(idToken);

    if (const AppError *error = isError(&googleAuthResult))
    {
        return std::move(*error);
    }

    GoogleTokenInfo googleLoginResult = std::get<GoogleTokenInfo>(std::move(googleAuthResult));

    UserDb &&saveUserDb = UserDb{
        .firstName = std::move(googleLoginResult.given_name),
        .lastName = std::move(googleLoginResult.given_name),
        .alias = std::move(googleLoginResult.name),
        .email = std::move(googleLoginResult.email),
        .googleId = std::move(googleLoginResult.sub),
        .settings = {
            .defaultCurrency = "USD"}};

    saveUserDb.id = "user_" + boost::uuids::to_string(boost::uuids::random_generator()());

    auto userDbResult = _userRepository.get()->createUserByGoogleIdIfNotExist(std::move(saveUserDb));

    if (const AppError *error = isError(&userDbResult))
    {
        return std::move(*error);
    }

    auto user = std::get<UserDb>(std::move(userDbResult));

    auto accessToken = _jwtService.get()->createUserToken(user.id);

    return std::move(UserLoginResult{
        .accessToken = std::move(accessToken),
        .sessionToken = "",
        .user = std::move(user)});
}
