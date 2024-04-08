#pragma once

#include <string>

#include <App.h>

namespace app::auth
{
	void handleGoogleLogin(uWS::HttpResponse<false> *res, uWS::HttpRequest *req);
}
