package com.airatkhisamiev.accounts.feature.receipts

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.airatkhisamiev.accounts.core.Money
import com.airatkhisamiev.accounts.core.Receipt
import com.airatkhisamiev.accounts.core.ReceiptsCore
import com.airatkhisamiev.accounts.ui.common.rfc3339Month
import com.airatkhisamiev.accounts.ui.common.userMessage
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch

class ReceiptListViewModel(private val core: ReceiptsCore) : ViewModel() {
    /** Receipts of one month, newest month first, with the month's total per currency. */
    data class MonthGroup(
        val month: String, // "2026-10"
        val totals: List<Money>,
        val receipts: List<Receipt>
    )

    data class UiState(
        val months: List<MonthGroup> = emptyList(),
        val refreshing: Boolean = false,
        val message: String? = null
    )

    private val mutableState = MutableStateFlow(UiState())
    val state = mutableState.asStateFlow()

    /** Shows what is on the device; fast and works offline. */
    fun load() {
        viewModelScope.launch {
            runCatching { core.listReceipts() }
                .onSuccess { receipts -> mutableState.update { it.copy(months = groupByMonth(receipts)) } }
                .onFailure { error -> mutableState.update { it.copy(message = error.userMessage()) } }
        }
    }

    /** Pull to refresh: sync with the server, then show the result. */
    fun refresh() {
        mutableState.update { it.copy(refreshing = true, message = null) }
        viewModelScope.launch {
            val failure = runCatching { core.sync() }.exceptionOrNull()
            mutableState.update { it.copy(refreshing = false, message = failure?.userMessage()) }
            load()
        }
    }
}

/** Pure function, so it can be unit-tested without the core. */
fun groupByMonth(receipts: List<Receipt>): List<ReceiptListViewModel.MonthGroup> = receipts
    .groupBy { rfc3339Month(it.purchasedAt) }
    .toSortedMap(reverseOrder())
    .map { (month, inMonth) ->
        val totals =
            inMonth
                .groupBy { it.money.currency }
                .map { (currency, sameCurrency) -> Money(sameCurrency.sumOf { it.money.amountMinor }, currency) }
        ReceiptListViewModel.MonthGroup(month, totals, inMonth)
    }
