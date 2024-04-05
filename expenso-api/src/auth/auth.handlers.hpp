#pragma once

#include <string>

#include "App.h"

#include "users/users.models.hpp"

namespace app
{
	namespace auth
	{
		void handleGoogleLogin(uWS::HttpResponse<false> *res, uWS::HttpRequest *req);
	}
}
