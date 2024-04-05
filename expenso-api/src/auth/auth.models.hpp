#pragma once

#include <string>

#include "users/users.models.hpp"

namespace app
{
	namespace auth
	{
		struct GoogleLoginRequest
		{
			std::string idToken;
		};

		struct UserLoginResult
		{
			std::string accessToken;
			std::string sessionToken;

			app::users::UserDb user;
		};
	}
}
