#pragma once

// JSON <-> receipt types: the API's wire format in one place.
// Field names are snake_case; timestamps are RFC 3339 strings.

#include "http/JsonFields.hpp"
#include "receipts/Receipt.hpp"

#include <boost/json/value.hpp>

namespace accounts::receipts {

boost::json::value toJson(const Receipt &receipt);
boost::json::value toJson(const ReceiptPage &page);

Result<ReceiptDraft> draftFromJson(const boost::json::object &body);
Result<ReceiptChanges> changesFromJson(const boost::json::object &body);

} // namespace accounts::receipts
