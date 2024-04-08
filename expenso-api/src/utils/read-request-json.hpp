#pragma once

#include <boost/json.hpp>
#include <App.h>

#include "utils/error.hpp"
#include "utils/enumToString.hpp"

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

    class RequestJsonBodyReader
    {
        std::string _buffer;

    public:
        template <class TBody>
        void read(uWS::HttpResponse<false> *response, void (*handler)(uWS::HttpResponse<false> *, TBody body))
        {
            _buffer.reserve(4096);

            response->onAborted([]() {})->onData(
                [this, handler, response](std::string_view data, bool isLast) mutable
                {
                    _buffer.append(data);

                    if (isLast)
                    {
                        try
                        {
                            auto jsonBodyObject = boost::json::parse(_buffer);

                            auto &&reqBody = boost::json::try_value_to<app::auth::GoogleLoginRequest>(std::move(jsonBodyObject));

                            if (reqBody.has_error())
                            {
                                app::error::abort(
                                    response,
                                    std::move(
                                        app::shared::AppError{
                                            .code = app::utils::enumToString(app::shared::AppErrorCode::PARSE_ERROR),
                                            .message = std::string(reqBody.error().message())}));
                                return;
                            }

                            handler(response, std::move(reqBody.value()));
                        }
                        catch (std::exception &ex)
                        {
                            // ToDo log me later
                            std::cerr << "Request caught error " << ex.what() << std::endl;

                            app::error::abort(
                                response,
                                std::move(
                                    app::shared::AppError{
                                        .code = app::utils::enumToString(app::shared::AppErrorCode::PARSE_ERROR),
                                        .message = "Invalid input json object"}));
                        }
                    }
                });
        }
    };

    // void equestBodyReader(uWS::HttpResponse<false> *response, std::string &buffer, RequestHandler handler);

}
