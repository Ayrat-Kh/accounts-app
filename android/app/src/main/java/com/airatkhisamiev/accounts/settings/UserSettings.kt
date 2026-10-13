package com.airatkhisamiev.accounts.settings

import android.content.Context
import androidx.core.content.edit

/** The user's preferences that the UI owns. */
class UserSettings(context: Context) {
    private val preferences = context.getSharedPreferences("settings", Context.MODE_PRIVATE)

    /** ISO 4217 code used for receipts that print no currency. Read by the core at startup. */
    var defaultCurrency: String
        get() = preferences.getString(KEY_DEFAULT_CURRENCY, null) ?: "EUR"
        set(value) = preferences.edit { putString(KEY_DEFAULT_CURRENCY, value) }

    private companion object {
        const val KEY_DEFAULT_CURRENCY = "default_currency"
    }
}
