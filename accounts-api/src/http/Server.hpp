#pragma once

// The HTTP server: uWebSockets, running on the same boost::asio::io_context
// as every coroutine in the program. (uSockets is built with its Asio
// backend; see vcpkg-overlays/usockets.) One thread, one event loop: uWS
// callbacks, Postgres waits, and our handlers all take turns on it, so no
// locks are needed anywhere.
//
// How one request flows:
//   1. uWS calls onRequest() synchronously. We copy method, path, query and
//      headers into an owned Request, because uWS::HttpRequest dies when the
//      callback returns.
//   2. The body arrives in chunks through onData(); the route's size limit is
//      enforced as it grows.
//   3. With the full body, a coroutine runs the route handler.
//   4. When the handler finishes, the response is written, but only if the
//      client is still there. uWS reports a vanished client through
//      onAborted(), after which the HttpResponse pointer must not be touched.
//
// TLS is terminated by Traefik in front of us, so this speaks plain HTTP.

#include "http/Router.hpp"

#include <boost/asio/awaitable.hpp>
#include <boost/asio/io_context.hpp>

#include <uwebsockets/App.h>

#include <memory>
#include <string>

namespace accounts::http {

class Server {
public:
    Server(boost::asio::io_context &ioContext, const Router &router);

    // Port 0 picks a free port (tests). Throws if the port cannot be bound.
    void listen(const std::string &host, std::uint16_t port);

    // Serves requests until stop() was called and all connections are closed.
    void run();

    // Stops accepting new connections; requests in flight still finish.
    void stop();

    std::uint16_t port() const;

private:
    struct Exchange; // one request/response pair in flight

    void onRequest(uWS::HttpResponse<false> *response, uWS::HttpRequest *request);
    boost::asio::awaitable<void> handle(uWS::HttpResponse<false> *response, std::shared_ptr<Exchange> exchange,
                                        RouteMatch match);

    boost::asio::io_context &ioContext_;
    const Router &router_;
    uWS::App app_;
    us_listen_socket_t *listenSocket_ = nullptr;
};

} // namespace accounts::http
