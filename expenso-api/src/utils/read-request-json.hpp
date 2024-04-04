#pragma once

#include <functional>

#include <boost/json.hpp>

#include "App.h"

namespace app
{
    namespace utils
    {
        using RequestHandler = std::function<void(boost::json::value json_response)>;

        void ReadRequestJson(uWS::HttpResponse<false> *response, RequestHandler);
    }
}
