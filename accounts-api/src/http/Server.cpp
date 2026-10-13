#include "http/Server.hpp"

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>

#include <charconv>
#include <stdexcept>

namespace accounts::http {

namespace asio = boost::asio;
using asio::awaitable;
using UwsResponse = uWS::HttpResponse<false>;

struct Server::Exchange {
    Request request;
    std::size_t maxBodyBytes = 0;
    // Set by uWS's onAborted. Once true, the UwsResponse* is dangling.
    bool aborted = false;
};

namespace {

// Attaches uWebSockets to our io_context. Must run before the first uWS::App
// is created on this thread, hence a function called from the constructor's
// member initialiser list.
uWS::App makeAppOnLoop(asio::io_context &ioContext) {
    uWS::Loop::get(&ioContext);
    return uWS::App();
}

Request copyRequest(uWS::HttpRequest &request) {
    Request copy;
    copy.method = parseMethod(request.getCaseSensitiveMethod());
    copy.path = std::string(request.getUrl());
    copy.queryParams = parseQueryString(request.getQuery());
    for (const auto [name, value] : request) {
        copy.headers.emplace(std::string(name), std::string(value));
    }
    return copy;
}

std::optional<std::size_t> declaredContentLength(const Request &request) {
    const auto header = request.header("content-length");
    std::size_t length = 0;
    if (header.empty() || std::from_chars(header.data(), header.data() + header.size(), length).ec != std::errc{}) {
        return std::nullopt;
    }
    return length;
}

// Writes a complete response. Called both from inside uWS callbacks and from
// our coroutines; cork() batches the writes into one syscall in both cases.
void writeResponse(UwsResponse *uws, const Response &response, bool closeConnection = false) {
    uws->cork([&] {
        uws->writeStatus(statusLine(response.status));
        for (const auto &[name, value] : response.headers) {
            uws->writeHeader(name, value);
        }
        // end() adds Content-Length itself.
        uws->end(response.body, closeConnection);
    });
}

} // namespace

Server::Server(asio::io_context &ioContext, const Router &router)
    : ioContext_(ioContext), router_(router), app_(makeAppOnLoop(ioContext)) {
    app_.any("/*", [this](UwsResponse *response, uWS::HttpRequest *request) { onRequest(response, request); });
}

void Server::listen(const std::string &host, std::uint16_t port) {
    app_.listen(host, port, [this](us_listen_socket_t *socket) { listenSocket_ = socket; });
    if (listenSocket_ == nullptr) {
        throw std::runtime_error("cannot listen on " + host + ":" + std::to_string(port));
    }
}

void Server::run() {
    // An io_context that once ran out of work stays "stopped" until
    // restarted (main.cpp runs the migrations on it first); uWS would then
    // return immediately.
    ioContext_.restart();
    // uWS drives the io_context (run_one in a loop, with its own pre/post
    // hooks) for as long as it has sockets open.
    app_.run();
}

void Server::stop() {
    if (listenSocket_ != nullptr) {
        us_listen_socket_close(0, listenSocket_);
        listenSocket_ = nullptr;
    }
}

std::uint16_t Server::port() const {
    return static_cast<std::uint16_t>(us_socket_local_port(0, reinterpret_cast<us_socket_t *>(listenSocket_)));
}

void Server::onRequest(UwsResponse *response, uWS::HttpRequest *request) {
    auto exchange = std::make_shared<Exchange>();
    exchange->request = copyRequest(*request);

    auto match = router_.match(exchange->request.method, exchange->request.path);
    if (!match) {
        writeResponse(response, errorResponse({ErrorCode::NotFound, "no such endpoint"}));
        return;
    }

    exchange->maxBodyBytes = router_.maxBodyBytes(*match);
    if (declaredContentLength(exchange->request).value_or(0) > exchange->maxBodyBytes) {
        // Refuse before reading a single body byte, and close: the unread
        // body is still on the wire.
        writeResponse(response, errorResponse({ErrorCode::PayloadTooLarge, "request body is too large"}), true);
        return;
    }

    response->onAborted([exchange] { exchange->aborted = true; });

    response->onData([this, response, exchange, match = std::move(*match)](std::string_view chunk,
                                                                           bool isLast) mutable {
        if (exchange->aborted) {
            return;
        }
        std::string &body = exchange->request.body;
        if (body.size() + chunk.size() > exchange->maxBodyBytes) {
            // Only reachable for chunked uploads without Content-Length.
            writeResponse(response, errorResponse({ErrorCode::PayloadTooLarge, "request body is too large"}), true);
            exchange->aborted = true;  // already answered: ignore further chunks
            return;
        }
        body.append(chunk);
        if (isLast) {
            asio::co_spawn(ioContext_, handle(response, exchange, std::move(match)), asio::detached);
        }
    });
}

awaitable<void> Server::handle(UwsResponse *response, std::shared_ptr<Exchange> exchange, RouteMatch match) {
    Response result = co_await router_.dispatch(match, std::move(exchange->request));
    // Same thread as uWS, so nothing can abort the request between this
    // check and the write.
    if (!exchange->aborted) {
        writeResponse(response, result);
    }
}

} // namespace accounts::http
