#pragma once

#include <App.h>

namespace app::users
{
    void handleGetUserById(uWS::HttpResponse<false> *res, uWS::HttpRequest *req);
}
