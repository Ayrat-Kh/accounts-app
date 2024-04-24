#pragma once

#include <App.h>

namespace accounts
{
    void handleGetUserById(uWS::HttpResponse<false> *res, uWS::HttpRequest *req);
}
