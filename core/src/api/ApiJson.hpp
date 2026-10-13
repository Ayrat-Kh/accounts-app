#pragma once

// The API's JSON wire format <-> core types, in one place.

#include "receipts/Types.hpp"

#include <nlohmann/json.hpp>

namespace receipts::api {

nlohmann::json receiptToJson(const Receipt &receipt); // request body of POST /v1/receipts
Receipt receiptFromJson(const nlohmann::json &json);  // throws nlohmann::json::exception on malformed input

} // namespace receipts::api
