#pragma once

#include <string>

#include <boost/describe.hpp>

#include "accounts/shared/models.hpp"

namespace accounts::auth
{
	struct GoogleLoginRequest
	{
		std::string idToken;
	};
	BOOST_DESCRIBE_STRUCT(GoogleLoginRequest, (), (idToken))

	struct GoogleLoginValidationResult
	{
		std::string idToken;
	};
	BOOST_DESCRIBE_STRUCT(GoogleLoginValidationResult, (), (idToken))

	struct UserLoginResult
	{
		std::string accessToken;
		std::string sessionToken;

		::accounts::shared::UserDb user;
	};
	BOOST_DESCRIBE_STRUCT(UserLoginResult, (), (accessToken, sessionToken, user))

}
