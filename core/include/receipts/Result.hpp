#pragma once

// Errors cross the library boundary as values, never as exceptions, so the
// JNI layer and Swift can translate them without try/catch gymnastics.

#include <expected>
#include <string>

namespace receipts {

enum class ErrorCode {
    InvalidInput,  // the caller passed something unusable (e.g. an unreadable image)
    NotSignedIn,   // the operation needs an account
    Network,       // no connection, timeout, TLS failure
    Server,        // the server answered with an error
    NotFound,
    ModelMissing,  // OCR or embedding files have not been downloaded yet
    Internal,      // a bug
};

struct Error {
    ErrorCode code;
    std::string message;
};

template <class T>
using Result = std::expected<T, Error>;

inline std::unexpected<Error> fail(ErrorCode code, std::string message) {
    return std::unexpected(Error{code, std::move(message)});
}

} // namespace receipts
