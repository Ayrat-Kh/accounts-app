package com.airatkhisamiev.accounts.feature.models

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.airatkhisamiev.accounts.core.CoreException
import com.airatkhisamiev.accounts.core.ReceiptsCore
import com.airatkhisamiev.accounts.ui.common.userMessage
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch

/** Downloads the scanner models once (~350 MB); interrupted downloads resume. */
class ModelsViewModel(private val core: ReceiptsCore) : ViewModel() {
    data class UiState(
        val downloading: Boolean = false,
        val bytesDone: Long = 0,
        val bytesTotal: Long = 0,
        val ready: Boolean = false,
        val error: String? = null
    ) {
        val progress: Float get() = if (bytesTotal > 0) bytesDone.toFloat() / bytesTotal else 0f
    }

    private val mutableState = MutableStateFlow(UiState())
    val state = mutableState.asStateFlow()

    fun download() {
        mutableState.update { it.copy(downloading = true, error = null) }
        viewModelScope.launch {
            try {
                core.downloadModels { done, total ->
                    mutableState.update { it.copy(bytesDone = done, bytesTotal = total) }
                }
                mutableState.update { it.copy(downloading = false, ready = true) }
            } catch (error: CoreException) {
                mutableState.update { it.copy(downloading = false, error = error.userMessage()) }
            }
        }
    }
}
