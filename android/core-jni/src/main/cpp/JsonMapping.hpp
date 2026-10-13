#pragma once

// Values cross the JNI boundary as JSON strings. Mapping every field of every
// struct through JNI calls would take hundreds of fragile lines; JSON keeps
// both sides plain: nlohmann here, kotlinx.serialization in Kotlin
// (CoreModels.kt). Field names are snake_case on both sides.

#include "receipts/Config.hpp"
#include "receipts/Types.hpp"

#include <nlohmann/json.hpp>

namespace receipts::jni {

CoreConfig configFromJson(const nlohmann::json &json);
ReceiptDraft draftFromJson(const nlohmann::json &json);

nlohmann::json toJson(const ScanResult &result);
nlohmann::json toJson(const Receipt &receipt);
nlohmann::json toJson(const SyncReport &report);

} // namespace receipts::jni
