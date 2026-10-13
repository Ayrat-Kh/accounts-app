package com.airatkhisamiev.accounts.feature.signin

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.airatkhisamiev.accounts.core.CoreException
import com.airatkhisamiev.accounts.core.ReceiptsCore
import com.airatkhisamiev.accounts.ui.common.userMessage
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch

class SignInViewModel(private val core: ReceiptsCore) : ViewModel() {
    data class UiState(val busy: Boolean = false, val error: String? = null, val signedIn: Boolean = false)

    private val mutableState = MutableStateFlow(UiState())
    val state = mutableState.asStateFlow()

    /** Called with the ID token that Credential Manager returned. */
    fun signIn(googleIdToken: String) {
        mutableState.update { it.copy(busy = true, error = null) }
        viewModelScope.launch {
            try {
                core.signInWithGoogle(googleIdToken)
                mutableState.update { it.copy(busy = false, signedIn = true) }
            } catch (error: CoreException) {
                mutableState.update { it.copy(busy = false, error = error.userMessage()) }
            }
        }
    }

    fun showError(message: String) = mutableState.update { it.copy(busy = false, error = message) }
}
