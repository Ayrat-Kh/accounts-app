#include "JsonMapping.hpp"

namespace receipts::jni {

using nlohmann::json;

namespace {

template <class T>
json optionalToJson(const std::optional<T> &value) {
    return value ? json(*value) : json(nullptr);
}

template <class T>
std::optional<T> optionalFrom(const json &object, const char *key) {
    if (!object.contains(key) || object.at(key).is_null()) {
        return std::nullopt;
    }
    return object.at(key).get<T>();
}

json toJson(const Money &money) {
    return {{"amount_minor", money.amountMinor}, {"currency", money.currency}};
}

Money moneyFromJson(const json &object) {
    return {object.at("amount_minor").get<std::int64_t>(), object.at("currency").get<std::string>()};
}

const char *sourceName(ExtractionSource source) {
    switch (source) {
    case ExtractionSource::Embedding: return "EMBEDDING";
    case ExtractionSource::Keywords: return "KEYWORDS";
    case ExtractionSource::Llm: return "LLM";
    }
    return "KEYWORDS";
}

const char *syncStateName(SyncState state) {
    switch (state) {
    case SyncState::Synced: return "SYNCED";
    case SyncState::PendingUpload: return "PENDING_UPLOAD";
    case SyncState::PendingDelete: return "PENDING_DELETE";
    }
    return "PENDING_UPLOAD";
}

} // namespace

CoreConfig configFromJson(const json &object) {
    CoreConfig config;
    config.apiBaseUrl = object.at("api_base_url").get<std::string>();
    config.dataDirectory = object.at("data_directory").get<std::string>();
    config.defaultCurrency = object.value("default_currency", "");
    config.ocrLanguages = object.value("ocr_languages", config.ocrLanguages);
    config.caCertificatesDirectory = optionalFrom<std::string>(object, "ca_certificates_directory");
    return config;
}

ReceiptDraft draftFromJson(const json &object) {
    return {
        .money = moneyFromJson(object.at("money")),
        .purchasedAt = object.at("purchased_at").get<std::string>(),
        .merchant = optionalFrom<std::string>(object, "merchant"),
        .ocrConfidence = optionalFrom<double>(object, "ocr_confidence"),
        .imagePath = optionalFrom<std::string>(object, "image_path"),
    };
}

json toJson(const ScanResult &result) {
    json candidates = json::array();
    for (const auto &candidate : result.candidates) {
        candidates.push_back({{"money", toJson(candidate.money)}, {"line_text", candidate.lineText}});
    }
    return {
        {"total", result.total ? toJson(*result.total) : json(nullptr)},
        {"purchased_at", optionalToJson(result.purchasedAt)},
        {"merchant", optionalToJson(result.merchant)},
        {"confidence", result.confidence},
        {"source", sourceName(result.source)},
        {"candidates", candidates},
        {"ocr_lines", result.ocrLines},
    };
}

json toJson(const Receipt &receipt) {
    return {
        {"local_id", receipt.localId},
        {"server_id", optionalToJson(receipt.serverId)},
        {"money", toJson(receipt.money)},
        {"purchased_at", receipt.purchasedAt},
        {"merchant", optionalToJson(receipt.merchant)},
        {"ocr_confidence", optionalToJson(receipt.ocrConfidence)},
        {"image_path", optionalToJson(receipt.imagePath)},
        {"has_image_on_server", receipt.hasImageOnServer},
        {"sync_state", syncStateName(receipt.syncState)},
    };
}

json toJson(const SyncReport &report) {
    return {{"uploaded", report.uploaded},
            {"deleted", report.deleted},
            {"downloaded", report.downloaded},
            {"failed", report.failed}};
}

} // namespace receipts::jni
