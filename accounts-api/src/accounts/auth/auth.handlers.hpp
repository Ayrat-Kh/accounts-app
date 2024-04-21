#pragma once

#include <string>

#include <App.h>

namespace accounts
{
	void handleGoogleLogin(uWS::HttpResponse<false> *res, uWS::HttpRequest *req);
}
