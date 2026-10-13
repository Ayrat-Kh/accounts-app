package com.airatkhisamiev.accounts.core

import kotlinx.serialization.SerialName
import kotlinx.serialization.Serializable

// The core's value types as they cross JNI (JSON written by JsonMapping.cpp).

/** Money in minor units: 12.34 EUR is Money(1234, "EUR"). */
@Serializable
data class Money(@SerialName("amount_minor") val amountMinor: Long, val currency: String)

@Serializable
enum class ExtractionSource { EMBEDDING, KEYWORDS, LLM }

@Serializable
data class AmountCandidate(val money: Money, @SerialName("line_text") val lineText: String)

@Serializable
data class ScanResult(
    val total: Money?,
    @SerialName("purchased_at") val purchasedAt: String?,
    val merchant: String?,
    val confidence: Double,
    val source: ExtractionSource,
    val candidates: List<AmountCandidate>,
    @SerialName("ocr_lines") val ocrLines: List<String>
)

@Serializable
enum class SyncState { SYNCED, PENDING_UPLOAD, PENDING_DELETE }

@Serializable
data class Receipt(
    @SerialName("local_id") val localId: String,
    @SerialName("server_id") val serverId: String?,
    val money: Money,
    @SerialName("purchased_at") val purchasedAt: String,
    val merchant: String?,
    @SerialName("ocr_confidence") val ocrConfidence: Double?,
    @SerialName("image_path") val imagePath: String?,
    @SerialName("has_image_on_server") val hasImageOnServer: Boolean,
    @SerialName("sync_state") val syncState: SyncState
)

@Serializable
data class ReceiptDraft(
    val money: Money,
    @SerialName("purchased_at") val purchasedAt: String,
    val merchant: String?,
    @SerialName("ocr_confidence") val ocrConfidence: Double?,
    @SerialName("image_path") val imagePath: String?
)

@Serializable
data class SyncReport(val uploaded: Int, val deleted: Int, val downloaded: Int, val failed: Int)

@Serializable
data class CoreConfig(
    @SerialName("api_base_url") val apiBaseUrl: String,
    @SerialName("data_directory") val dataDirectory: String,
    @SerialName("default_currency") val defaultCurrency: String = "",
    @SerialName("ocr_languages") val ocrLanguages: String = "eng+deu+rus",
    // Android's system trust store, in the hashed layout OpenSSL understands.
    @SerialName("ca_certificates_directory") val caCertificatesDirectory: String? = "/system/etc/security/cacerts"
)
