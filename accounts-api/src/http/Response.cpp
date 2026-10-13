#include "http/Response.hpp"

#include <boost/json/serialize.hpp>
#include <spdlog/spdlog.h>

namespace accounts::http {

namespace {

struct ErrorDescription {
    Status status;
    std::string_view code;
};

ErrorDescription describe(ErrorCode code) {
    switch (code) {
    case ErrorCode::InvalidInput: return {Status::BadRequest, "invalid_input"};
    case ErrorCode::Unauthorized: return {Status::Unauthorized, "unauthorized"};
    case ErrorCode::Forbidden: return {Status::Forbidden, "forbidden"};
    case ErrorCode::NotFound: return {Status::NotFound, "not_found"};
    case ErrorCode::PayloadTooLarge: return {Status::PayloadTooLarge, "payload_too_large"};
    case ErrorCode::UpstreamFailure: return {Status::BadGateway, "upstream_failure"};
    case ErrorCode::Internal: return {Status::InternalServerError, "internal"};
    }
    return {Status::InternalServerError, "internal"};
}

bool isServerSide(ErrorCode code) {
    return code == ErrorCode::UpstreamFailure || code == ErrorCode::Internal;
}

} // namespace

std::string statusLine(Status status) {
    const auto reason = [status]() -> std::string_view {
        switch (status) {
        case Status::Ok: return "OK";
        case Status::Created: return "Created";
        case Status::NoContent: return "No Content";
        case Status::PartialContent: return "Partial Content";
        case Status::BadRequest: return "Bad Request";
        case Status::Unauthorized: return "Unauthorized";
        case Status::Forbidden: return "Forbidden";
        case Status::NotFound: return "Not Found";
        case Status::PayloadTooLarge: return "Payload Too Large";
        case Status::RangeNotSatisfiable: return "Range Not Satisfiable";
        case Status::InternalServerError: return "Internal Server Error";
        case Status::BadGateway: return "Bad Gateway";
        }
        return "Unknown";
    }();
    return std::to_string(static_cast<int>(status)) + " " + std::string(reason);
}

Response bytesResponse(Status status, std::string bytes, std::string_view contentType) {
    Response response{status, {}, std::move(bytes)};
    response.withHeader("Content-Type", std::string(contentType)).withHeader("Cache-Control", "no-store");
    return response;
}

Response jsonResponse(Status status, const boost::json::value &body) {
    return bytesResponse(status, boost::json::serialize(body), "application/json");
}

Response noContentResponse() {
    return {Status::NoContent, {}, {}};
}

Response errorResponse(const AppError &error) {
    const auto [status, code] = describe(error.code);
    // Server-side failures may carry SQL or S3 details: log them, but never
    // send them to the client.
    std::string message = error.message;
    if (isServerSide(error.code)) {
        spdlog::error("{}: {}", code, error.message);
        message = "something went wrong on our side";
    }
    return jsonResponse(status, {{"error", {{"code", code}, {"message", message}}}});
}

} // namespace accounts::http
