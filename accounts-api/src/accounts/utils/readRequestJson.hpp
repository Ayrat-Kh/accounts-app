#pragma once

#include <boost/json.hpp>
#include <App.h>

#include "accounts/utils/error.hpp"
#include "accounts/utils/enumHelpers.hpp"
#include "accounts/utils/jsonSerialize.hpp"

namespace accounts
{
    using RequestHandler = void(uWS::HttpResponse<false> *res, boost::json::value json_response);

    class RequestJsonBodyReader
    {
        std::string _json;

    public:
        template <class TBody>
        void read(uWS::HttpResponse<false> *response, uWS::HttpRequest *req, void (*handler)(uWS::HttpResponse<false> *, uWS::HttpRequest *req, TBody body))
        {
            _json.reserve(4096);

            response->onAborted([]() {})->onData(
                [this, handler, response, req](std::string_view data, bool isLast) mutable
                {
                    _json.append(data);

                    if (!isLast)
                    {
                        return;
                    }

                    try
                    {
                        TBody reqBody;

                        boost::system::error_code ec;

                        boost::json::parse_into(reqBody, _json, ec);

                        if (ec.failed())
                        {
                            auto message = std::move(ec.what());

                            std::cerr << message << std::endl;

                            abort(
                                response,
                                std::move(
                                    AppError{
                                        .code = enumToString(EAppErrorCode::PARSE_BODY_ERROR),
                                        .message = std::move(message)}));
                            return;
                        }

                        handler(response, req, std::move(reqBody));
                    }
                    catch (std::exception &ex)
                    {
                        // ToDo log me later
                        std::cerr << "Request caught error " << ex.what() << std::endl;

                        abort(
                            response,
                            std::move(
                                AppError{
                                    .code = enumToString(EAppErrorCode::PARSE_BODY_ERROR),
                                    .message = "Invalid input json object"}));
                    }
                });
        }
    };
}
