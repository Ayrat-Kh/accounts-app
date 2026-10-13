#pragma once

// Reads typed fields from a JSON request body and remembers the first problem,
// so a whole DTO can be read in one readable block:
//
//     JsonFields fields(body);
//     NewThing thing{
//         .name = fields.requiredString("name"),
//         .count = fields.optionalInt64("count"),
//     };
//     if (auto error = fields.firstError()) return std::unexpected(*error);
//
// A missing or mistyped field yields a default value plus a recorded
// InvalidInput error, so callers check once at the end instead of after
// every field.

#include "util/Error.hpp"

#include <boost/json/object.hpp>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace accounts::http {

// Parses `body` as a JSON object; anything else is an InvalidInput error.
Result<boost::json::object> parseJsonObject(std::string_view body);

class JsonFields {
public:
    explicit JsonFields(const boost::json::object &object) : object_(object) {}

    std::string requiredString(std::string_view key);
    std::optional<std::string> optionalString(std::string_view key);
    std::int64_t requiredInt64(std::string_view key);
    std::optional<std::int64_t> optionalInt64(std::string_view key);
    std::optional<double> optionalDouble(std::string_view key);

    const std::optional<AppError> &firstError() const { return firstError_; }

private:
    const boost::json::value *find(std::string_view key) const; // nullptr if absent or null
    void recordError(std::string_view key, std::string_view expected);

    const boost::json::object &object_;
    std::optional<AppError> firstError_;
};

} // namespace accounts::http
