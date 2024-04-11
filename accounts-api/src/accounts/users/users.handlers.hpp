#pragma once

#include <App.h>

namespace accounts::users
{
    void handleGetUserById(uWS::HttpResponse<false> *res, uWS::HttpRequest *req);
}
