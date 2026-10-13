package com.airatkhisamiev.accounts.feature.detail

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.airatkhisamiev.accounts.core.Receipt
import com.airatkhisamiev.accounts.core.ReceiptsCore
import com.airatkhisamiev.accounts.sync.SyncScheduler
import com.airatkhisamiev.accounts.ui.common.userMessage
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch

class ReceiptDetailViewModel(
    private val core: ReceiptsCore,
    private val syncScheduler: SyncScheduler,
    private val localId: String
) : ViewModel() {
    data class UiState(val receipt: Receipt? = null, val deleted: Boolean = false, val error: String? = null)

    private val mutableState = MutableStateFlow(UiState())
    val state = mutableState.asStateFlow()

    init {
        viewModelScope.launch {
            runCatching { core.listReceipts().firstOrNull { it.localId == localId } }
                .onSuccess { receipt -> mutableState.update { it.copy(receipt = receipt) } }
                .onFailure { error -> mutableState.update { it.copy(error = error.userMessage()) } }
        }
    }

    fun delete() {
        viewModelScope.launch {
            runCatching { core.deleteReceipt(localId) }
                .onSuccess {
                    syncScheduler.requestSync()
                    mutableState.update { it.copy(deleted = true) }
                }.onFailure { error -> mutableState.update { it.copy(error = error.userMessage()) } }
        }
    }
}
