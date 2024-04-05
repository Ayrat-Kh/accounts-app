#include "read-request-json.hpp"

void app::utils::requestBodyReader(uWS::HttpResponse<false> *response, std::string &buffer, RequestHandler handler)
{
    buffer.reserve(4096);

    response->onAborted([]() {})->onData(
        [buffer, handler, response](std::string_view data, bool isLast) mutable
        {
            buffer.append(data);

            if (isLast)
            {
                try
                {
                    auto value = boost::json::parse(buffer);

                    handler(response, value);
                }
                catch (std::exception &ex)
                {
                    // ToDo log me later
                    std::cerr << "Request caught error " << ex.what() << std::endl;

                    boost::json::object obj({{"message", "Invalid input json object"}});

                    response->writeStatus("400")->end("unknown error");
                }
            }
        });
}
