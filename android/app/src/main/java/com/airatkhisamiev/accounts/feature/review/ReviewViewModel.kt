package com.airatkhisamiev.accounts.feature.review

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.airatkhisamiev.accounts.core.AmountCandidate
import com.airatkhisamiev.accounts.core.CoreException
import com.airatkhisamiev.accounts.core.Money
import com.airatkhisamiev.accounts.core.ReceiptDraft
import com.airatkhisamiev.accounts.core.ReceiptsCore
import com.airatkhisamiev.accounts.money.CURRENCY_CODE_LENGTH
import com.airatkhisamiev.accounts.money.amountText
import com.airatkhisamiev.accounts.money.parseAmountMinor
import com.airatkhisamiev.accounts.sync.SyncScheduler
import com.airatkhisamiev.accounts.ui.common.rfc3339Date
import com.airatkhisamiev.accounts.ui.common.userMessage
import java.time.LocalDate
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch

/**
 * Scans the photo, pre-fills the form with what the core found, and saves
 * what the user confirms. The user always has the last word.
 */
class ReviewViewModel(
    private val core: ReceiptsCore,
    private val photoPath: String,
    private val syncScheduler: SyncScheduler,
    defaultCurrency: String
) : ViewModel() {
    data class UiState(
        val scanning: Boolean = true,
        val amount: String = "",
        val currency: String = "",
        val date: String = LocalDate.now().toString(), // "YYYY-MM-DD"
        val merchant: String = "",
        val confidence: Double? = null,
        val alternatives: List<AmountCandidate> = emptyList(),
        val saving: Boolean = false,
        val saved: Boolean = false,
        val error: String? = null
    )

    private val mutableState = MutableStateFlow(UiState(currency = defaultCurrency))
    val state = mutableState.asStateFlow()

    init {
        viewModelScope.launch {
            try {
                val scan = core.scanReceipt(photoPath)
                mutableState.update { current ->
                    current.copy(
                        scanning = false,
                        amount = scan.total?.let { amountText(it.amountMinor) } ?: "",
                        currency = scan.total?.currency?.ifEmpty { null } ?: current.currency,
                        date = scan.purchasedAt?.let(::rfc3339Date) ?: current.date,
                        merchant = scan.merchant ?: "",
                        confidence = scan.confidence,
                        alternatives = scan.candidates.distinctBy { it.money.amountMinor }.take(MAX_ALTERNATIVES)
                    )
                }
            } catch (error: CoreException) {
                // Scanning is a convenience: the user can still type everything in.
                mutableState.update { it.copy(scanning = false, error = error.userMessage()) }
            }
        }
    }

    fun onAmountChange(value: String) = mutableState.update { it.copy(amount = value) }

    fun onCurrencyChange(value: String) = mutableState.update {
        it.copy(currency = value.uppercase().take(CURRENCY_CODE_LENGTH))
    }

    fun onDateChange(value: String) = mutableState.update { it.copy(date = value) }

    fun onMerchantChange(value: String) = mutableState.update { it.copy(merchant = value) }

    fun onAlternativeChosen(candidate: AmountCandidate) =
        mutableState.update { it.copy(amount = amountText(candidate.money.amountMinor)) }

    fun save() {
        val current = state.value
        val draft = validDraft(current)
        if (draft == null) {
            mutableState.update { it.copy(error = "Please enter an amount, a 3-letter currency and a date.") }
            return
        }
        mutableState.update { it.copy(saving = true, error = null) }
        viewModelScope.launch {
            try {
                core.saveReceipt(draft)
                syncScheduler.requestSync() // uploads now, or as soon as there is a network
                mutableState.update { it.copy(saving = false, saved = true) }
            } catch (error: CoreException) {
                mutableState.update { it.copy(saving = false, error = error.userMessage()) }
            }
        }
    }

    /** The form as a draft, or null while something is missing or malformed. */
    private fun validDraft(state: UiState): ReceiptDraft? {
        val amountMinor = parseAmountMinor(state.amount)
        val date = runCatching { LocalDate.parse(state.date) }.getOrNull()
        val currencyValid = Regex("[A-Z]{3}").matches(state.currency)
        if (amountMinor == null || date == null || !currencyValid) {
            return null
        }
        return ReceiptDraft(
            money = Money(amountMinor, state.currency),
            purchasedAt = "${date}T12:00:00.000Z", // noon UTC, the core's convention for date-only receipts
            merchant = state.merchant.trim().ifEmpty { null },
            ocrConfidence = state.confidence,
            imagePath = photoPath
        )
    }

    private companion object {
        const val MAX_ALTERNATIVES = 4
    }
}
