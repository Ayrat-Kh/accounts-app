#pragma once

#include "net/HttpTransport.hpp"

#include <optional>

namespace receipts::net {

// HttpTransport over libcurl. Each call uses its own curl handle, so it is
// safe to call from several threads at once.
class CurlHttpTransport : public HttpTransport {
public:
    // `caCertificatesDirectory`: see CoreConfig::caCertificatesDirectory.
    explicit CurlHttpTransport(std::optional<std::string> caCertificatesDirectory = std::nullopt);

    Result<HttpResponse> send(const HttpRequest &request) override;

private:
    std::optional<std::string> caCertificatesDirectory_;
};

} // namespace receipts::net
