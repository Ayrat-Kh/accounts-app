package com.airatkhisamiev.accounts

import com.airatkhisamiev.accounts.core.Money
import com.airatkhisamiev.accounts.core.Receipt
import com.airatkhisamiev.accounts.core.SyncState
import com.airatkhisamiev.accounts.feature.receipts.groupByMonth
import org.junit.Assert.assertEquals
import org.junit.Test

class GroupByMonthTest {
    private fun receipt(purchasedAt: String, amountMinor: Long, currency: String = "EUR") = Receipt(
        localId = "$purchasedAt-$amountMinor",
        serverId = null,
        money = Money(amountMinor, currency),
        purchasedAt = purchasedAt,
        merchant = null,
        ocrConfidence = null,
        imagePath = null,
        hasImageOnServer = false,
        syncState = SyncState.SYNCED
    )

    @Test
    fun `receipts are grouped by month, newest month first`() {
        val months =
            groupByMonth(
                listOf(
                    receipt("2026-09-30T12:00:00.000Z", 100),
                    receipt("2026-10-01T12:00:00.000Z", 200),
                    receipt("2026-10-13T12:00:00.000Z", 300)
                )
            )
        assertEquals(listOf("2026-10", "2026-09"), months.map { it.month })
        assertEquals(listOf(Money(500, "EUR")), months.first().totals)
    }

    @Test
    fun `different currencies are totalled separately, never added together`() {
        val totals =
            groupByMonth(
                listOf(
                    receipt("2026-10-01T12:00:00.000Z", 100, "EUR"),
                    receipt("2026-10-02T12:00:00.000Z", 50, "USD")
                )
            ).single().totals
        assertEquals(setOf(Money(100, "EUR"), Money(50, "USD")), totals.toSet())
    }
}
