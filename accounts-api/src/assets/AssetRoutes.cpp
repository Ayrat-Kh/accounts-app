#include "assets/AssetRoutes.hpp"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <format>

namespace accounts::assets {

using boost::asio::awaitable;
using http::Method;
using http::Request;
using http::Response;
using http::Status;

namespace {

constexpr std::uint64_t maxRangeBytes = 8ULL * 1024 * 1024;
constexpr std::string_view manifestKey = "assets/manifest.json";

// Asset names are plain file names; this also rules out "../" tricks.
bool isValidAssetName(std::string_view name) {
    return !name.empty() && name.size() <= 128 && name.front() != '.' && std::ranges::all_of(name, [](char c) {
        return std::isalnum(static_cast<unsigned char>(c)) || c == '.' || c == '_' || c == '-';
    });
}

std::optional<std::uint64_t> parseNumber(std::string_view text) {
    std::uint64_t value = 0;
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (error != std::errc{} || end != text.data() + text.size()) {
        return std::nullopt;
    }
    return value;
}

awaitable<Response> getManifest(storage::ObjectStore &objectStore, Request &) {
    auto manifest = co_await objectStore.get(std::string(manifestKey));
    if (!manifest) {
        co_return http::errorResponse(manifest.error());
    }
    if (!*manifest) {
        co_return http::errorResponse({ErrorCode::NotFound, "no assets have been published"});
    }
    co_return http::bytesResponse(Status::Ok, std::move((*manifest)->bytes), "application/json");
}

awaitable<Response> getAssetRange(storage::ObjectStore &objectStore, Request &request) {
    const std::string_view name = request.pathParam("name");
    if (!isValidAssetName(name)) {
        co_return http::errorResponse({ErrorCode::NotFound, "no such asset"});
    }
    // Without a Range header, the first piece is served; its Content-Range
    // tells the client the total size.
    const auto requested = request.header("range").empty() ? storage::ByteRange{0, maxRangeBytes - 1}
                                                           : parseRangeHeader(request.header("range"));
    if (!requested || requested->last - requested->first + 1 > maxRangeBytes) {
        co_return http::errorResponse({ErrorCode::InvalidInput, "Range must be bytes=first-last, at most 8 MiB"});
    }

    auto object = co_await objectStore.get("assets/" + std::string(name), requested);
    if (!object) {
        co_return http::errorResponse(object.error());
    }
    if (!*object) {
        co_return http::errorResponse({ErrorCode::NotFound, "no such asset"});
    }
    const storage::StoredObject &piece = **object;
    if (requested->first >= piece.totalSize) {
        Response response{Status::RangeNotSatisfiable, {}, {}};
        co_return response.withHeader("Content-Range", std::format("bytes */{}", piece.totalSize));
    }

    const std::uint64_t last = requested->first + piece.bytes.size() - 1;
    auto response = http::bytesResponse(Status::PartialContent, piece.bytes, "application/octet-stream");
    co_return response.withHeader("Content-Range",
                                  std::format("bytes {}-{}/{}", requested->first, last, piece.totalSize));
}

} // namespace

std::optional<storage::ByteRange> parseRangeHeader(std::string_view header) {
    constexpr std::string_view unit = "bytes=";
    if (!header.starts_with(unit)) {
        return std::nullopt;
    }
    const std::string_view spec = header.substr(unit.size());
    const auto dash = spec.find('-');
    if (dash == std::string_view::npos) {
        return std::nullopt;
    }
    const auto first = parseNumber(spec.substr(0, dash));
    const auto last = parseNumber(spec.substr(dash + 1));
    if (!first || !last || *last < *first) {
        return std::nullopt;
    }
    return storage::ByteRange{*first, *last};
}

void registerAssetRoutes(http::Router &router, storage::ObjectStore &objectStore) {
    auto bind = [&objectStore](auto handler) {
        return [&objectStore, handler](Request &request) { return handler(objectStore, request); };
    };
    router.add(Method::Get, "/v1/assets", bind(getManifest));
    router.add(Method::Get, "/v1/assets/:name", bind(getAssetRange));
}

} // namespace accounts::assets
