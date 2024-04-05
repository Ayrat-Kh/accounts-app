#include "read-request-json.hpp"

app::utils::RequestBodyReader::RequestBodyReader(RequestHandler handler, uWS::HttpResponse<false> *response)
    : _handler(std::move(handler)),
      _response(response)
{
    _buffer.reserve(4096);
}

void app::utils::RequestBodyReader::read()
{
    _response
        ->onAborted(
            []() {})
        ->onData(
            [this](std::string_view data, bool isLast) mutable
            {
                _buffer.append(data);

                if (isLast)
                {
                    try
                    {
                        auto value = boost::json::parse(_buffer);

                        _handler(value);
                    }
                    catch (std::exception &ex)
                    {
                        // ToDo log me later
                        std::cerr << "Request caught error " << ex.what() << std::endl;

                        boost::json::object obj({{"message", "Invalid input json object"}});

                        _response->writeStatus("400")->end("unknown error");
                    }
                }
            });
}
