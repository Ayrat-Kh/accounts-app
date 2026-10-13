#pragma once

// Maps "METHOD /path/:param" to a coroutine handler, and runs the shared
// pre-flight that every endpoint needs, so handlers don't repeat it:
//   - authentication (on by default; opt out per route),
//   - the per-route body size limit (checked by the Server while reading),
//   - turning an escaped exception into a 500 instead of a dropped connection.

#include "auth/AuthUser.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"

#include <boost/asio/awaitable.hpp>

#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace accounts::http {

// Decides who is calling, given the Authorization header. Implemented by
// auth::AccessTokenService; an interface so the router doesn't depend on JWTs.
class Authenticator {
public:
    virtual ~Authenticator() = default;
    virtual std::optional<AuthUser> authenticate(std::string_view authorizationHeader) const = 0;
};

// Routes are registered once at startup and live as long as the Router, so
// handler lambdas may capture services by reference.
using Handler = std::function<boost::asio::awaitable<Response>(Request &)>;

struct RouteOptions {
    bool requiresAuth = true;
    std::optional<std::size_t> maxBodyBytes;  // unset: the router's default (small JSON bodies)
};

struct Route {
    Method method;
    std::vector<std::string> segments;  // "/v1/receipts/:id" -> {"v1", "receipts", ":id"}
    Handler handler;
    RouteOptions options;
};

struct RouteMatch {
    const Route *route;
    StringMap pathParams;
};

class Router {
public:
    Router(const Authenticator &authenticator, std::size_t defaultMaxBodyBytes);

    void add(Method method, std::string_view pattern, Handler handler, RouteOptions options = {});

    // `path` is without the query string.
    std::optional<RouteMatch> match(Method method, std::string_view path) const;

    // The largest body this route accepts. The Server checks it while the
    // body streams in, so an oversized upload is cut off instead of buffered.
    std::size_t maxBodyBytes(const RouteMatch &match) const;

    // Authenticates (if the route requires it) and runs the handler.
    boost::asio::awaitable<Response> dispatch(const RouteMatch &match, Request request) const;

private:
    const Authenticator &authenticator_;
    std::size_t defaultMaxBodyBytes_;
    std::vector<Route> routes_;
};

// Helpers for turning a raw request line into a Request; exposed for tests.
std::vector<std::string> splitPath(std::string_view path);
StringMap parseQueryString(std::string_view query);
Method parseMethod(std::string_view method);  // case-insensitive: "get" and "GET" both work

} // namespace accounts::http
