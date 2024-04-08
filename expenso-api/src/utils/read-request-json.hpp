#pragma once

#include <boost/json.hpp>
#include <App.h>

namespace app::utils
{
    using RequestHandler = void(uWS::HttpResponse<false> *res, boost::json::value json_response);

    struct RequestLambda
    {
        template <class F>
        static auto cify(F &&f)
        {
            static F fn = std::forward<F>(f);

            return [](uWS::HttpResponse<false> *res, boost::json::value json_response)
            {
                return fn(res, json_response);
            };
        }
    };

    void requestBodyReader(uWS::HttpResponse<false> *response, std::string &buffer, RequestHandler handler);

}
