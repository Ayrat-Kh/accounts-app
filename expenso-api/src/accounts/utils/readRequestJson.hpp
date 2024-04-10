#pragma once

#include <boost/json.hpp>
#include <App.h>

#include "accounts/utils/error.hpp"
#include "accounts/utils/enumToString.hpp"
#include "accounts/utils/jsonSerialize.hpp"

namespace accounts::utils
{
    using RequestHandler = void(uWS::HttpResponse<false> *res, boost::json::value json_response);

    class RequestJsonBodyReader
    {
        std::string _buffer;

    public:
        template <class TBody>
        void read(uWS::HttpResponse<false> *response, uWS::HttpRequest *req, void (*handler)(uWS::HttpResponse<false> *, uWS::HttpRequest *req, TBody body))
        {
            using namespace ::accounts::shared;
            using namespace ::accounts::error;

            _buffer.reserve(4096);

            response->onAborted([]() {})->onData(
                [this, handler, response, req](std::string_view data, bool isLast) mutable
                {
                    _buffer.append(data);

                    if (isLast)
                    {
                        try
                        {
                            auto jsonBodyObject = boost::json::parse(_buffer);

                            auto &&reqBody = boost::json::try_value_to<TBody>(std::move(jsonBodyObject));

                            if (reqBody.has_error())
                            {
                                abort(
                                    response,
                                    std::move(
                                        AppError{
                                            .code = enumToString(AppErrorCode::PARSE_BODY_ERROR),
                                            .message = std::string(reqBody.error().message())}));
                                return;
                            }

                            handler(response, req, std::move(reqBody.value()));
                        }
                        catch (std::exception &ex)
                        {
                            // ToDo log me later
                            std::cerr << "Request caught error " << ex.what() << std::endl;

                            abort(
                                response,
                                std::move(
                                    AppError{
                                        .code = enumToString(AppErrorCode::PARSE_BODY_ERROR),
                                        .message = "Invalid input json object"}));
                        }
                    }
                });
        }
    };
}