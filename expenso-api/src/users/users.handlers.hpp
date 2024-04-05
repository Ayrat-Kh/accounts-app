#pragma once

#include <App.h>

namespace app
{
    namespace users
    {
        void handleGetUserById(uWS::HttpResponse<false> *res, uWS::HttpRequest *req);
    }
}
