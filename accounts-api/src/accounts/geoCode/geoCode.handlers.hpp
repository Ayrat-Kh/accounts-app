#pragma once

#include <App.h>

namespace accounts
{
    void handleDecodeGeoApi(uWS::HttpResponse<false> *res, uWS::HttpRequest *req);
}
