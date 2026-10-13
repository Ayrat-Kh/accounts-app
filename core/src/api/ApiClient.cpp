#include "api/ApiClient.hpp"

#include "api/ApiJson.hpp"
#include "util/Files.hpp"

#include <charconv>
#include <format>

namespace receipts::api {

using nlohmann::json;

namespace {

constexpr const char *accessTokenKey = "accounts.access_token";
constexpr const char *refreshTokenKey = "accounts.refresh_token";
constexpr auto uploadTimeout = std::chrono::seconds(120);  // a 10 MB photo on a slow uplink

net::HttpRequest jsonRequest(std::string method, std::string url, const json &body) {
    return {.method = std::move(method),
            .url = std::move(url),
            .headers = {{"Content-Type", "application/json"}},
            .body = body.dump()};
}

// Wraps "parse this JSON body" so a malformed server answer becomes an Error, not an exception.
template <class Parse>
auto parseBody(const net::HttpResponse &response, Parse parse) -> Result<decltype(parse(json{}))> {
    try {
        return parse(json::parse(response.body));
    } catch (const json::exception &error) {
        return fail(ErrorCode::Server, std::string("unexpected response from the server: ") + error.what());
    }
}

} // namespace

Error errorFromResponse(const net::HttpResponse &response) {
    std::string message = "server answered " + std::to_string(response.status);
    const auto body = json::parse(response.body, nullptr, /*allow_exceptions=*/false);
    if (body.is_object() && body.contains("error") && body["error"].contains("message")) {
        message = body["error"]["message"].get<std::string>();
    }
    switch (response.status) {
    case 400: return {ErrorCode::InvalidInput, message};
    case 401:
    case 403: return {ErrorCode::NotSignedIn, message};
    case 404: return {ErrorCode::NotFound, message};
    default: return {ErrorCode::Server, message};
    }
}

ApiClient::ApiClient(std::shared_ptr<net::HttpTransport> transport, std::string baseUrl,
                     std::shared_ptr<SecureStore> secureStore)
    : transport_(std::move(transport)), baseUrl_(std::move(baseUrl)), secureStore_(std::move(secureStore)) {}

bool ApiClient::isSignedIn() const {
    return secureStore_->read(refreshTokenKey).has_value();
}

Result<void> ApiClient::storeTokens(const net::HttpResponse &response) {
    auto tokens = parseBody(response, [](const json &body) {
        return std::pair{body.at("access_token").get<std::string>(), body.at("refresh_token").get<std::string>()};
    });
    if (!tokens) {
        return std::unexpected(tokens.error());
    }
    secureStore_->write(accessTokenKey, tokens->first);
    secureStore_->write(refreshTokenKey, tokens->second);
    return {};
}

Result<void> ApiClient::signInWithGoogle(const std::string &idToken) {
    auto response = send(jsonRequest("POST", baseUrl_ + "/auth/google", {{"id_token", idToken}}), false);
    if (!response) {
        return std::unexpected(response.error());
    }
    return storeTokens(*response);
}

void ApiClient::signOut() {
    if (auto refreshToken = secureStore_->read(refreshTokenKey)) {
        (void)send(jsonRequest("POST", baseUrl_ + "/auth/signout", {{"refresh_token", *refreshToken}}), false);
    }
    secureStore_->remove(accessTokenKey);
    secureStore_->remove(refreshTokenKey);
}

Result<void> ApiClient::refreshTokens() {
    const auto refreshToken = secureStore_->read(refreshTokenKey);
    if (!refreshToken) {
        return fail(ErrorCode::NotSignedIn, "not signed in");
    }
    auto response = sendOnce(jsonRequest("POST", baseUrl_ + "/auth/token", {{"refresh_token", *refreshToken}}), false);
    if (!response) {
        return std::unexpected(response.error());
    }
    if (response->status == 401) {
        // The session ended (expired or revoked): the user must sign in again.
        secureStore_->remove(accessTokenKey);
        secureStore_->remove(refreshTokenKey);
        return fail(ErrorCode::NotSignedIn, "session expired, please sign in again");
    }
    if (!response->isSuccess()) {
        return std::unexpected(errorFromResponse(*response));
    }
    return storeTokens(*response);
}

Result<net::HttpResponse> ApiClient::sendOnce(const net::HttpRequest &request, bool authenticated) {
    net::HttpRequest withAuth = request;
    if (authenticated) {
        const auto accessToken = secureStore_->read(accessTokenKey);
        if (!accessToken) {
            return fail(ErrorCode::NotSignedIn, "not signed in");
        }
        withAuth.headers["Authorization"] = "Bearer " + *accessToken;
    }
    return transport_->send(withAuth);
}

Result<net::HttpResponse> ApiClient::send(net::HttpRequest request, bool authenticated) {
    auto response = sendOnce(request, authenticated);
    if (response && authenticated && response->status == 401) {
        // Access tokens live 15 minutes: refresh once, then retry.
        std::lock_guard lock(refreshMutex_);
        if (auto refreshed = refreshTokens(); !refreshed) {
            return std::unexpected(refreshed.error());
        }
        response = sendOnce(request, authenticated);
    }
    if (response && !response->isSuccess()) {
        return std::unexpected(errorFromResponse(*response));
    }
    return response;
}

Result<Receipt> ApiClient::createReceipt(const Receipt &receipt) {
    auto response = send(jsonRequest("POST", baseUrl_ + "/v1/receipts", receiptToJson(receipt)));
    if (!response) {
        return std::unexpected(response.error());
    }
    return parseBody(*response, receiptFromJson);
}

Result<void> ApiClient::uploadImage(const std::string &serverId, const std::string &bytes) {
    auto response = send({.method = "PUT",
                          .url = baseUrl_ + "/v1/receipts/" + serverId + "/image",
                          .headers = {{"Content-Type", "application/octet-stream"},
                                      {"X-Content-SHA256", files::sha256Hex(bytes)}},
                          .body = bytes,
                          .timeout = uploadTimeout});
    if (!response) {
        return std::unexpected(response.error());
    }
    return {};
}

Result<void> ApiClient::deleteReceipt(const std::string &serverId) {
    auto response = send({.method = "DELETE", .url = baseUrl_ + "/v1/receipts/" + serverId});
    if (!response && response.error().code != ErrorCode::NotFound) {
        return std::unexpected(response.error());
    }
    return {};
}

Result<std::vector<Receipt>> ApiClient::listAllReceipts() {
    std::vector<Receipt> receipts;
    std::optional<std::string> cursor;
    do {
        std::string url = baseUrl_ + "/v1/receipts?limit=100";
        if (cursor) {
            url += "&cursor=" + *cursor;  // base64url: safe in a URL as is
        }
        auto response = send({.method = "GET", .url = url});
        if (!response) {
            return std::unexpected(response.error());
        }
        auto page = parseBody(*response, [&](const json &body) {
            for (const json &item : body.at("items")) {
                receipts.push_back(receiptFromJson(item));
            }
            return body.at("next_cursor").is_null() ? std::nullopt
                                                    : std::optional(body.at("next_cursor").get<std::string>());
        });
        if (!page) {
            return std::unexpected(page.error());
        }
        cursor = *page;
    } while (cursor);
    return receipts;
}

Result<std::vector<AssetInfo>> ApiClient::listAssets() {
    auto response = send({.method = "GET", .url = baseUrl_ + "/v1/assets"});
    if (!response) {
        return std::unexpected(response.error());
    }
    return parseBody(*response, [](const json &body) {
        std::vector<AssetInfo> assets;
        for (const json &item : body) {
            assets.push_back({item.at("name").get<std::string>(), item.at("size").get<std::uint64_t>(),
                              item.at("sha256").get<std::string>()});
        }
        return assets;
    });
}

Result<AssetPiece> ApiClient::downloadAssetPiece(const std::string &name, std::uint64_t first, std::uint64_t last) {
    auto response = send({.method = "GET",
                          .url = baseUrl_ + "/v1/assets/" + name,
                          .headers = {{"Range", std::format("bytes={}-{}", first, last)}},
                          .timeout = uploadTimeout});
    if (!response) {
        return std::unexpected(response.error());
    }
    // "Content-Range: bytes 0-8388607/309855456" -> 309855456
    const std::string &contentRange = response->headers["content-range"];
    const auto slash = contentRange.rfind('/');
    std::uint64_t totalSize = 0;
    if (slash == std::string::npos ||
        std::from_chars(contentRange.data() + slash + 1, contentRange.data() + contentRange.size(), totalSize).ec !=
            std::errc{}) {
        return fail(ErrorCode::Server, "asset response without a valid Content-Range");
    }
    return AssetPiece{std::move(response->body), totalSize};
}

} // namespace receipts::api
