#pragma once

// What a route handler returns, plus builders for every kind of response the
// API sends. Handlers never set status codes or headers by hand, which keeps
// the wire format uniform:
//   success -> JSON body (or raw bytes for images and assets)
//   failure -> {"error": {"code": "not_found", "message": "..."}}

#include "util/Error.hpp"

#include <boost/json/value.hpp>

#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace accounts::http {

enum class Status : int {
    Ok = 200,
    Created = 201,
    NoContent = 204,
    PartialContent = 206,
    BadRequest = 400,
    Unauthorized = 401,
    Forbidden = 403,
    NotFound = 404,
    PayloadTooLarge = 413,
    RangeNotSatisfiable = 416,
    InternalServerError = 500,
    BadGateway = 502,
};

// "404 Not Found", the form uWebSockets' writeStatus() expects.
std::string statusLine(Status status);

struct Response {
    Status status = Status::Ok;
    std::vector<std::pair<std::string, std::string>> headers;
    std::string body;

    Response &withHeader(std::string name, std::string value) {
        headers.emplace_back(std::move(name), std::move(value));
        return *this;
    }
};

Response jsonResponse(Status status, const boost::json::value &body);
Response bytesResponse(Status status, std::string bytes, std::string_view contentType);
Response noContentResponse();
Response errorResponse(const AppError &error);

// Turns a Result into a response: the value is serialised by `toJson` with
// `successStatus`; an error becomes errorResponse(). Most handlers end with it.
template <class T, class ToJson>
Response respond(const Result<T> &result, Status successStatus, ToJson toJson) {
    if (!result) {
        return errorResponse(result.error());
    }
    return jsonResponse(successStatus, toJson(*result));
}

} // namespace accounts::http
