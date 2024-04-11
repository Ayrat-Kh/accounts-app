#pragma once

#include <string>

#include <App.h>

namespace accounts::auth
{
	void handleGoogleLogin(uWS::HttpResponse<false> *res, uWS::HttpRequest *req);
}
