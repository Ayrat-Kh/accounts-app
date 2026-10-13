#pragma once

// A tiny HTTP/1.1 client (Boost.Beast) for driving the real server in
// HttpApiTest. One connection per request, closed afterwards.

#include <boost/asio/awaitable.hpp>
#include <boost/json/value.hpp>

#include <cstdint>
#include <map>
#include <string>

namespace accounts::testing {

struct TestResponse {
    int status = 0;
    std::map<std::string, std::string> headers;  // lower-case names
    std::string body;

    boost::json::value json() const;
};

struct TestRequest {
    std::string method = "GET";
    std::string target;
    std::string body;
    std::map<std::string, std::string> headers;
};

boost::asio::awaitable<TestResponse> sendRequest(std::uint16_t port, TestRequest request);

} // namespace accounts::testing
