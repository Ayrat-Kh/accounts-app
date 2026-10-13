#pragma once

// One HTTP exchange, synchronously. Everything that talks to a server (our
// API client, model downloads, OpenAI-compatible engines) depends on this
// interface, not on libcurl, so tests can script responses without sockets.

#include "receipts/Result.hpp"

#include <chrono>
#include <map>
#include <string>

namespace receipts::net {

struct HttpRequest {
    std::string method = "GET";
    std::string url;
    std::map<std::string, std::string> headers;
    std::string body;
    std::chrono::seconds timeout{30};
};

struct HttpResponse {
    int status = 0;
    std::map<std::string, std::string> headers; // names in lower case
    std::string body;

    bool isSuccess() const { return status >= 200 && status < 300; }
};

class HttpTransport {
public:
    virtual ~HttpTransport() = default;
    // A Network error means no HTTP response at all; any HTTP status, even
    // 500, is a successful exchange that the caller interprets.
    virtual Result<HttpResponse> send(const HttpRequest &request) = 0;
};

} // namespace receipts::net
