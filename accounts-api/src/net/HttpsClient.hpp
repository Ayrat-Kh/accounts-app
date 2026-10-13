#pragma once

// A minimal coroutine HTTPS client for calling third parties (Google).
// Certificates are verified against the system trust store and the host
// name, unlike the old sendHttpsRequest.hpp which accepted any certificate.

#include "util/Error.hpp"

#include <boost/asio/awaitable.hpp>
#include <boost/asio/ssl/context.hpp>

#include <string>

namespace accounts::net {

struct HttpsResponse {
    int status;
    std::string body;
};

class HttpsClient {
public:
    HttpsClient();

    // GET https://<host><target>, e.g. get("oauth2.googleapis.com", "/tokeninfo?id_token=...").
    boost::asio::awaitable<Result<HttpsResponse>> get(std::string host, std::string target);

private:
    boost::asio::ssl::context tlsContext_;
};

} // namespace accounts::net
