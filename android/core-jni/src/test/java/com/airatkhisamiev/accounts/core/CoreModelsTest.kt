package com.airatkhisamiev.accounts.core

import org.junit.Assert.assertEquals
import org.junit.Assert.assertNull
import org.junit.Assert.assertTrue
import org.junit.Test

/** The JSON below is exactly what JsonMapping.cpp writes. */
class CoreModelsTest {
    private val json = ReceiptsCore.json

    @Test
    fun `a scan result decodes, including a missing date`() {
        val scan =
            json.decodeFromString<ScanResult>(
                """
                {"total": {"amount_minor": 577, "currency": "EUR"}, "purchased_at": null,
                 "merchant": "REWE Markt", "confidence": 0.86, "source": "EMBEDDING",
                 "candidates": [{"money": {"amount_minor": 577, "currency": "EUR"}, "line_text": "SUMME EUR 5,77"}],
                 "ocr_lines": ["REWE Markt", "SUMME EUR 5,77"]}
                """.trimIndent()
            )
        assertEquals(Money(577, "EUR"), scan.total)
        assertNull(scan.purchasedAt)
        assertEquals(ExtractionSource.EMBEDDING, scan.source)
    }

    @Test
    fun `a receipt list decodes with its sync state`() {
        val receipts =
            json.decodeFromString<List<Receipt>>(
                """
                [{"local_id": "local-1", "server_id": null, "money": {"amount_minor": 1234, "currency": "EUR"},
                  "purchased_at": "2026-10-13T12:00:00.000Z", "merchant": null, "ocr_confidence": 0.9,
                  "image_path": "/data/images/local-1.jpg", "has_image_on_server": false,
                  "sync_state": "PENDING_UPLOAD"}]
                """.trimIndent()
            )
        assertEquals(SyncState.PENDING_UPLOAD, receipts.single().syncState)
    }

    @Test
    fun `the config encodes with the field names the core reads`() {
        val encoded = json.encodeToString(CoreConfig(apiBaseUrl = "https://api", dataDirectory = "/data"))
        assertTrue(encoded.contains("\"api_base_url\":\"https://api\""))
        assertTrue(encoded.contains("\"ca_certificates_directory\":\"/system/etc/security/cacerts\""))
    }

    @Test
    fun `unknown error codes from native code map to INTERNAL`() {
        assertEquals(CoreException.Code.NETWORK, CoreException("NETWORK", "offline").code)
        assertEquals(CoreException.Code.INTERNAL, CoreException("SOMETHING_NEW", "?").code)
    }
}
