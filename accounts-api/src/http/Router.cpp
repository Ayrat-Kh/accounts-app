#include "http/Router.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <cctype>

namespace accounts::http {

using boost::asio::awaitable;

namespace {

std::string percentDecode(std::string_view text) {
    auto hexValue = [](char c) -> int {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        return -1;
    };
    std::string decoded;
    decoded.reserve(text.size());
    for (std::size_t i = 0; i < text.size(); ++i) {
        const bool isEscape =
            text[i] == '%' && i + 2 < text.size() && hexValue(text[i + 1]) >= 0 && hexValue(text[i + 2]) >= 0;
        if (isEscape) {
            decoded.push_back(static_cast<char>(hexValue(text[i + 1]) * 16 + hexValue(text[i + 2])));
            i += 2;
        } else {
            decoded.push_back(text[i] == '+' ? ' ' : text[i]);
        }
    }
    return decoded;
}

// Splits on `delimiter` and drops empty pieces: "/a//b/" -> {"a", "b"}.
std::vector<std::string_view> split(std::string_view text, char delimiter) {
    std::vector<std::string_view> pieces;
    std::size_t start = 0;
    while (start <= text.size()) {
        const auto found = text.find(delimiter, start);
        const auto end = found == std::string_view::npos ? text.size() : found;
        if (end > start) {
            pieces.push_back(text.substr(start, end - start));
        }
        start = end + 1;
    }
    return pieces;
}

bool isParameter(const std::string &segment) {
    return !segment.empty() && segment.front() == ':';
}

} // namespace

std::vector<std::string> splitPath(std::string_view path) {
    std::vector<std::string> segments;
    for (std::string_view segment : split(path, '/')) {
        segments.emplace_back(segment);
    }
    return segments;
}

StringMap parseQueryString(std::string_view query) {
    StringMap params;
    for (std::string_view pair : split(query, '&')) {
        const auto equals = pair.find('=');
        const auto key = pair.substr(0, equals);
        const auto value = equals == std::string_view::npos ? std::string_view{} : pair.substr(equals + 1);
        params.emplace(percentDecode(key), percentDecode(value));
    }
    return params;
}

Method parseMethod(std::string_view method) {
    std::string upper(method);
    std::ranges::transform(upper, upper.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    if (upper == "GET") return Method::Get;
    if (upper == "HEAD") return Method::Head;
    if (upper == "POST") return Method::Post;
    if (upper == "PUT") return Method::Put;
    if (upper == "PATCH") return Method::Patch;
    if (upper == "DELETE") return Method::Delete;
    return Method::Other;
}

Router::Router(const Authenticator &authenticator, std::size_t defaultMaxBodyBytes)
    : authenticator_(authenticator), defaultMaxBodyBytes_(defaultMaxBodyBytes) {}

void Router::add(Method method, std::string_view pattern, Handler handler, RouteOptions options) {
    routes_.push_back({method, splitPath(pattern), std::move(handler), options});
}

std::optional<RouteMatch> Router::match(Method method, std::string_view path) const {
    const auto requestSegments = splitPath(path);
    for (const Route &route : routes_) {
        if (route.method != method || route.segments.size() != requestSegments.size()) {
            continue;
        }
        RouteMatch match{&route, {}};
        bool matches = true;
        for (std::size_t i = 0; i < route.segments.size() && matches; ++i) {
            if (isParameter(route.segments[i])) {
                match.pathParams.emplace(route.segments[i].substr(1), percentDecode(requestSegments[i]));
            } else {
                matches = route.segments[i] == requestSegments[i];
            }
        }
        if (matches) {
            return match;
        }
    }
    return std::nullopt;
}

std::size_t Router::maxBodyBytes(const RouteMatch &match) const {
    return match.route->options.maxBodyBytes.value_or(defaultMaxBodyBytes_);
}

awaitable<Response> Router::dispatch(const RouteMatch &match, Request request) const {
    request.pathParams = match.pathParams;

    if (match.route->options.requiresAuth) {
        request.user = authenticator_.authenticate(request.header("authorization"));
        if (!request.user) {
            co_return errorResponse({ErrorCode::Unauthorized, "a valid access token is required"});
        }
    }

    try {
        co_return co_await match.route->handler(request);
    } catch (const std::exception &error) {
        // Handlers report expected failures through Result; reaching this is a bug.
        spdlog::error("unhandled exception in {}: {}", request.path, error.what());
        co_return errorResponse({ErrorCode::Internal, error.what()});
    }
}

} // namespace accounts::http
