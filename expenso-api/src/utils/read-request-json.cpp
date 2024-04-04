#include "read-request-json.hpp"

void app::utils::ReadRequestJson(uWS::HttpResponse<false> *response, app::utils::RequestHandler handler)
{
    std::string requestData;
    requestData.reserve(4096);
    response->onData(
        [&](std::string_view data, bool isLast)
        {
            requestData.append(data);
            if (isLast)
            {
                try
                {
                    auto value = boost::json::parse(requestData);

                    handler(std::move(value));
                }
                catch (std::exception &ex)
                {
                    // ToDo log me later
                    std::cerr << ex.what();

                    boost::json::object obj;
                    obj["message"] = "Invalid input json object";

                    response->writeStatus("400")->end(boost::json::serialize(obj));
                }
            }
        });
}