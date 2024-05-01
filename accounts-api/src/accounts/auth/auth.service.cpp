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

boost::asio::awaitable<std::variant<UserLoginResult, AppError>> AuthServiceImpl::googleAuth(
    std::string_view idToken)
{
    accounts::AccountsResult<GoogleTokenInfo> googleAuthResult = co_await _googleLoginService.get()->getGoogleUser(idToken);

    if (googleAuthResult.has_error())
    {
        co_return std::move(googleAuthResult.error());
    }

    GoogleTokenInfo &googleLoginResult = googleAuthResult.value();

    GoogleUpsertUserDb saveUserDb;
    saveUserDb.id = "user_" + boost::uuids::to_string(boost::uuids::random_generator()());
    saveUserDb.firstName = std::move(googleLoginResult.given_name);
    saveUserDb.lastName = std::move(googleLoginResult.given_name);
    saveUserDb.alias = std::move(googleLoginResult.name);
    saveUserDb.email = std::move(googleLoginResult.email);
    saveUserDb.googleId = std::move(googleLoginResult.sub);
    saveUserDb.status = EUserStatus::CREATED;
    saveUserDb.settings = {
        .defaultCurrency = ECurrency::USD};

    auto userDbResult = _userRepository.get()->createUserByGoogleIdIfNotExist(std::move(saveUserDb));

    if (const AppError *error = isError(&userDbResult))
    {
        co_return std::move(*error);
    }

    auto user = std::get<UserDb>(std::move(userDbResult));

    auto accessToken = _jwtService.get()->createUserToken(user.id);

    co_return std::move(UserLoginResult{
        .accessToken = std::move(accessToken),
        .sessionToken = "",
        .user = std::move(user)});
}
