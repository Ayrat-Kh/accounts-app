package com.airatkhisamiev.accounts.feature.settings

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.airatkhisamiev.accounts.core.ReceiptsCore
import com.airatkhisamiev.accounts.money.CURRENCY_CODE_LENGTH
import com.airatkhisamiev.accounts.settings.UserSettings
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch

class SettingsViewModel(private val core: ReceiptsCore, private val settings: UserSettings) : ViewModel() {
    data class UiState(val defaultCurrency: String, val signedOut: Boolean = false)

    private val mutableState = MutableStateFlow(UiState(defaultCurrency = settings.defaultCurrency))
    val state = mutableState.asStateFlow()

    fun onDefaultCurrencyChange(value: String) {
        val currency = value.uppercase().take(CURRENCY_CODE_LENGTH)
        mutableState.update { it.copy(defaultCurrency = currency) }
        if (currency.length == CURRENCY_CODE_LENGTH) settings.defaultCurrency = currency
    }

    fun signOut() {
        viewModelScope.launch {
            core.signOut()
            mutableState.update { it.copy(signedOut = true) }
        }
    }
}
