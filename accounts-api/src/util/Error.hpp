#pragma once

// The one error type of the application. Every fallible operation returns
// Result<T>; the HTTP layer turns an AppError into a status code in exactly
// one place (http/Response.cpp).

#include <expected>
#include <string>

namespace accounts {

enum class ErrorCode {
    InvalidInput,     // 400: the request is malformed or fails validation
    Unauthorized,     // 401: missing, invalid or expired credentials
    Forbidden,        // 403: valid credentials, but not allowed (e.g. e-mail not on the allowlist)
    NotFound,         // 404: does not exist *for this user* (we never reveal other users' data)
    PayloadTooLarge,  // 413
    UpstreamFailure,  // 502: Google, Garage or Postgres failed
    Internal,         // 500: a bug
};

struct AppError {
    ErrorCode code;
    std::string message;
};

template <class T>
using Result = std::expected<T, AppError>;

// Shorthand for `return std::unexpected(AppError{...})`.
inline std::unexpected<AppError> fail(ErrorCode code, std::string message) {
    return std::unexpected(AppError{code, std::move(message)});
}

} // namespace accounts
