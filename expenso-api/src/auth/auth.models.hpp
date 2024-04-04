#pragma once

#include <string_view>

#include "users/users.models.hpp"

namespace app
{
	namespace auth
	{
		struct GoogleLoginRequest
		{
			std::string_view idToken;
		};

		struct UserLoginResult
		{
			std::string_view accessToken;
			std::string_view sessionToken;

			app::users::UserDb userDb;
		};
	}
}
