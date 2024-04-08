#pragma once

#include <App.h>

namespace app
{
    namespace accounts
    {
        void handleGetAccountsByUserId(uWS::HttpResponse<false> *res, uWS::HttpRequest *req);
    }
}
