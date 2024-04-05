#pragma once

#include <functional>
#include <string>
#include <iostream>

#include <boost/json.hpp>

#include "App.h"

namespace app
{
    namespace utils
    {
        class RequestBodyReader
        {
        public:
            // using RequestHandler = std::function<void()>;
            using RequestHandler = uWS::MoveOnlyFunction<void(boost::json::value json_response)>;

            RequestBodyReader(RequestHandler handler, uWS::HttpResponse<false> *response);

            void read();

        private:
            RequestHandler _handler;
            std::string _buffer;
            uWS::HttpResponse<false> *_response;
        };
    }
}
