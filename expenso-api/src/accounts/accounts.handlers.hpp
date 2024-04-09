#pragma once

#include <App.h>

namespace app
{
    namespace accounts
    {
        void handleGetAccountsByUserId(uWS::HttpResponse<false> *res, uWS::HttpRequest *req);
        void handleAddAccount(uWS::HttpResponse<false> *res, uWS::HttpRequest *req);
    }
}
