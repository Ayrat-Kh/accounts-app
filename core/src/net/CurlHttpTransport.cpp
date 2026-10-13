#include "net/CurlHttpTransport.hpp"

#include "util/Text.hpp"

#include <curl/curl.h>

#include <memory>
#include <mutex>

namespace receipts::net {

namespace {

// curl_global_init is not thread-safe and must run once per process.
void initialiseCurlOnce() {
    static std::once_flag once;
    std::call_once(once, [] { curl_global_init(CURL_GLOBAL_DEFAULT); });
}

std::size_t appendBody(char *data, std::size_t size, std::size_t count, void *response) {
    static_cast<HttpResponse *>(response)->body.append(data, size * count);
    return size * count;
}

// Called once per header line, e.g. "Content-Range: bytes 0-99/1000\r\n".
std::size_t appendHeader(char *data, std::size_t size, std::size_t count, void *response) {
    const std::string_view line(data, size * count);
    if (const auto colon = line.find(':'); colon != std::string_view::npos) {
        static_cast<HttpResponse *>(response)->headers[text::toLower(text::trim(line.substr(0, colon)))] =
            text::trim(line.substr(colon + 1));
    }
    return size * count;
}

using CurlHandle = std::unique_ptr<CURL, decltype(&curl_easy_cleanup)>;
using HeaderList = std::unique_ptr<curl_slist, decltype(&curl_slist_free_all)>;

} // namespace

CurlHttpTransport::CurlHttpTransport(std::optional<std::string> caCertificatesDirectory)
    : caCertificatesDirectory_(std::move(caCertificatesDirectory)) {
    initialiseCurlOnce();
}

Result<HttpResponse> CurlHttpTransport::send(const HttpRequest &request) {
    CurlHandle curl(curl_easy_init(), &curl_easy_cleanup);
    if (!curl) {
        return fail(ErrorCode::Internal, "curl_easy_init failed");
    }

    HeaderList headers(nullptr, &curl_slist_free_all);
    for (const auto &[name, value] : request.headers) {
        headers.reset(curl_slist_append(headers.release(), (name + ": " + value).c_str()));
    }

    HttpResponse response;
    curl_easy_setopt(curl.get(), CURLOPT_URL, request.url.c_str());
    curl_easy_setopt(curl.get(), CURLOPT_CUSTOMREQUEST, request.method.c_str());
    curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, headers.get());
    if (!request.body.empty()) {
        curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS, request.body.data());
        curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDSIZE_LARGE, static_cast<curl_off_t>(request.body.size()));
    }
    curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, static_cast<long>(request.timeout.count()));
    curl_easy_setopt(curl.get(), CURLOPT_CONNECTTIMEOUT, 10L);
    curl_easy_setopt(curl.get(), CURLOPT_NOSIGNAL, 1L);  // required for multi-threaded use
    curl_easy_setopt(curl.get(), CURLOPT_FOLLOWLOCATION, 0L);
    curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, appendBody);
    curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl.get(), CURLOPT_HEADERFUNCTION, appendHeader);
    curl_easy_setopt(curl.get(), CURLOPT_HEADERDATA, &response);
    // Certificate and host name checks are on by default in libcurl; only the
    // trust store location may differ per platform.
    if (caCertificatesDirectory_) {
        curl_easy_setopt(curl.get(), CURLOPT_CAPATH, caCertificatesDirectory_->c_str());
    }

    if (const CURLcode result = curl_easy_perform(curl.get()); result != CURLE_OK) {
        return fail(ErrorCode::Network, std::string("HTTP ") + request.method + " " + request.url + ": " +
                                            curl_easy_strerror(result));
    }
    long status = 0;
    curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &status);
    response.status = static_cast<int>(status);
    return response;
}

} // namespace receipts::net
