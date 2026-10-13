#include "api/ApiJson.hpp"

namespace receipts::api {

using nlohmann::json;

namespace {

template <class T>
json optionalToJson(const std::optional<T> &value) {
    return value ? json(*value) : json(nullptr);
}

template <class T>
std::optional<T> optionalFromJson(const json &object, const char *key) {
    if (!object.contains(key) || object.at(key).is_null()) {
        return std::nullopt;
    }
    return object.at(key).get<T>();
}

} // namespace

json receiptToJson(const Receipt &receipt) {
    return {
        {"amount_minor", receipt.money.amountMinor},
        {"currency", receipt.money.currency},
        {"purchased_at", receipt.purchasedAt},
        {"merchant", optionalToJson(receipt.merchant)},
        {"ocr_confidence", optionalToJson(receipt.ocrConfidence)},
    };
}

Receipt receiptFromJson(const json &object) {
    Receipt receipt;
    receipt.serverId = object.at("id").get<std::string>();
    receipt.money = {object.at("amount_minor").get<std::int64_t>(), object.at("currency").get<std::string>()};
    receipt.purchasedAt = object.at("purchased_at").get<std::string>();
    receipt.merchant = optionalFromJson<std::string>(object, "merchant");
    receipt.ocrConfidence = optionalFromJson<double>(object, "ocr_confidence");
    receipt.hasImageOnServer = object.contains("image") && !object.at("image").is_null();
    receipt.syncState = SyncState::Synced;
    return receipt;
}

} // namespace receipts::api
