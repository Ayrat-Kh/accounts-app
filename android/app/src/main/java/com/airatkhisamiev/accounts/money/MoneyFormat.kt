package com.airatkhisamiev.accounts.money

import com.airatkhisamiev.accounts.core.Money
import java.math.BigDecimal
import java.text.NumberFormat
import java.util.Currency
import java.util.Locale

/** "12,34 €" for Money(1234, "EUR") in a German locale; plain digits if the currency is unknown. */
fun formatMoney(money: Money, locale: Locale = Locale.getDefault()): String {
    val amount = BigDecimal.valueOf(money.amountMinor, 2)
    val currency = runCatching { Currency.getInstance(money.currency) }.getOrNull()
    if (currency == null) {
        return NumberFormat.getNumberInstance(locale).apply { minimumFractionDigits = 2 }.format(amount)
    }
    return NumberFormat.getCurrencyInstance(locale).apply { this.currency = currency }.format(amount)
}

/** ISO 4217 codes are three letters. */
const val CURRENCY_CODE_LENGTH = 3

/** The amount as the user edits it: "12.34" (always a dot, two decimals). */
fun amountText(amountMinor: Long): String = BigDecimal.valueOf(amountMinor, 2).toPlainString()

/**
 * Parses what the user typed into minor units: "12.34", "12,34" and "12" are
 * all accepted; anything else (letters, three decimals, negative) is null.
 */
fun parseAmountMinor(text: String): Long? {
    val normalized = text.trim().replace(',', '.')
    if (!Regex("""\d{1,10}(\.\d{1,2})?""").matches(normalized)) {
        return null
    }
    return BigDecimal(normalized).movePointRight(2).longValueExact()
}
