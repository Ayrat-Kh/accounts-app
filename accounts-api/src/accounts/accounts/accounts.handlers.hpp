#pragma once

#include <App.h>

namespace accounts
{
    void handleGetAccountsByUserId(uWS::HttpResponse<false> *res, uWS::HttpRequest *req);
    void handleAddAccount(uWS::HttpResponse<false> *res, uWS::HttpRequest *req);
    void handleUpdateAccount(uWS::HttpResponse<false> *res, uWS::HttpRequest *req);
}
