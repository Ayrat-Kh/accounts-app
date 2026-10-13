#include "http/JsonFields.hpp"

#include <boost/json/parse.hpp>

namespace accounts::http {

Result<boost::json::object> parseJsonObject(std::string_view body) {
    boost::system::error_code error;
    boost::json::value parsed = boost::json::parse(body, error);
    if (error) {
        return fail(ErrorCode::InvalidInput, "request body is not valid JSON");
    }
    if (!parsed.is_object()) {
        return fail(ErrorCode::InvalidInput, "request body must be a JSON object");
    }
    return std::move(parsed.as_object());
}

const boost::json::value *JsonFields::find(std::string_view key) const {
    const auto *value = object_.if_contains(key);
    return (value == nullptr || value->is_null()) ? nullptr : value;
}

void JsonFields::recordError(std::string_view key, std::string_view expected) {
    if (!firstError_) {
        firstError_ = AppError{ErrorCode::InvalidInput, "field '" + std::string(key) + "' must be " +
                                                            std::string(expected)};
    }
}

std::string JsonFields::requiredString(std::string_view key) {
    if (auto value = optionalString(key)) {
        return *value;
    }
    recordError(key, "a string");
    return {};
}

std::optional<std::string> JsonFields::optionalString(std::string_view key) {
    const auto *value = find(key);
    if (value == nullptr) {
        return std::nullopt;
    }
    if (!value->is_string()) {
        recordError(key, "a string");
        return std::nullopt;
    }
    return std::string(value->as_string());
}

std::int64_t JsonFields::requiredInt64(std::string_view key) {
    if (auto value = optionalInt64(key)) {
        return *value;
    }
    recordError(key, "an integer");
    return 0;
}

std::optional<std::int64_t> JsonFields::optionalInt64(std::string_view key) {
    const auto *value = find(key);
    if (value == nullptr) {
        return std::nullopt;
    }
    if (value->is_int64()) {
        return value->as_int64();
    }
    if (value->is_uint64() && value->as_uint64() <= static_cast<std::uint64_t>(INT64_MAX)) {
        return static_cast<std::int64_t>(value->as_uint64());
    }
    recordError(key, "an integer");
    return std::nullopt;
}

std::optional<double> JsonFields::optionalDouble(std::string_view key) {
    const auto *value = find(key);
    if (value == nullptr) {
        return std::nullopt;
    }
    if (value->is_double()) {
        return value->as_double();
    }
    if (value->is_int64()) {
        return static_cast<double>(value->as_int64());
    }
    recordError(key, "a number");
    return std::nullopt;
}

} // namespace accounts::http
