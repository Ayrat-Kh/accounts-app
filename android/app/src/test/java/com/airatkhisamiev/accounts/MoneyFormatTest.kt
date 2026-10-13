package com.airatkhisamiev.accounts

import com.airatkhisamiev.accounts.core.Money
import com.airatkhisamiev.accounts.money.amountText
import com.airatkhisamiev.accounts.money.formatMoney
import com.airatkhisamiev.accounts.money.parseAmountMinor
import java.util.Locale
import org.junit.Assert.assertEquals
import org.junit.Assert.assertNull
import org.junit.Test

class MoneyFormatTest {
    @Test
    fun `typed amounts accept a dot or a comma`() {
        assertEquals(1234L, parseAmountMinor("12.34"))
        assertEquals(1234L, parseAmountMinor("12,34"))
        assertEquals(1200L, parseAmountMinor("12"))
        assertEquals(1250L, parseAmountMinor(" 12.5 "))
    }

    @Test
    fun `invalid amounts are rejected`() {
        assertNull(parseAmountMinor(""))
        assertNull(parseAmountMinor("12.345"))
        assertNull(parseAmountMinor("-1"))
        assertNull(parseAmountMinor("abc"))
    }

    @Test
    fun `minor units round-trip through the edit text`() {
        assertEquals("12.34", amountText(1234))
        assertEquals("0.05", amountText(5))
        assertEquals(1234L, parseAmountMinor(amountText(1234)))
    }

    @Test
    fun `money is formatted for the locale, with or without a known currency`() {
        assertEquals("€12.34", formatMoney(Money(1234, "EUR"), Locale.US))
        assertEquals("12.34", formatMoney(Money(1234, ""), Locale.US))
    }
}
