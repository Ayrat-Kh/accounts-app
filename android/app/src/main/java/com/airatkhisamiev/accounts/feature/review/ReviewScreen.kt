package com.airatkhisamiev.accounts.feature.review

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.AssistChip
import androidx.compose.material3.Button
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.input.KeyboardType
import androidx.compose.ui.unit.dp
import androidx.lifecycle.compose.collectAsStateWithLifecycle
import com.airatkhisamiev.accounts.money.formatMoney
import com.airatkhisamiev.accounts.ui.common.ReceiptPhoto

private const val UNSURE_BELOW = 0.6

@Composable
fun ReviewScreen(viewModel: ReviewViewModel, photoPath: String, onSaved: () -> Unit) {
    val state by viewModel.state.collectAsStateWithLifecycle()
    LaunchedEffect(state.saved) {
        if (state.saved) onSaved()
    }

    Column(
        modifier = Modifier.fillMaxSize().verticalScroll(rememberScrollState()).padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(12.dp)
    ) {
        ReceiptPhoto(photoPath, Modifier.fillMaxWidth())
        if (state.scanning) {
            Row(verticalAlignment = Alignment.CenterVertically, horizontalArrangement = Arrangement.spacedBy(12.dp)) {
                CircularProgressIndicator()
                Text("Reading the receipt…")
            }
            return@Column
        }
        state.confidence?.let { confidence ->
            if (confidence < UNSURE_BELOW) {
                Text("Not sure about the total, please check it.", color = MaterialTheme.colorScheme.tertiary)
            }
        }
        Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            OutlinedTextField(
                value = state.amount,
                onValueChange = viewModel::onAmountChange,
                label = { Text("Total") },
                keyboardOptions = KeyboardOptions(keyboardType = KeyboardType.Decimal),
                modifier = Modifier.weight(2f)
            )
            OutlinedTextField(
                value = state.currency,
                onValueChange = viewModel::onCurrencyChange,
                label = { Text("Currency") },
                modifier = Modifier.weight(1f)
            )
        }
        if (state.alternatives.size > 1) {
            Text("Other amounts on the receipt:", style = MaterialTheme.typography.labelLarge)
            Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                state.alternatives.forEach { candidate ->
                    AssistChip(
                        onClick = { viewModel.onAlternativeChosen(candidate) },
                        label = { Text(formatMoney(candidate.money)) }
                    )
                }
            }
        }
        OutlinedTextField(
            value = state.date,
            onValueChange = viewModel::onDateChange,
            label = { Text("Date (YYYY-MM-DD)") },
            modifier = Modifier.fillMaxWidth()
        )
        OutlinedTextField(
            value = state.merchant,
            onValueChange = viewModel::onMerchantChange,
            label = { Text("Shop") },
            modifier = Modifier.fillMaxWidth()
        )
        state.error?.let { Text(it, color = MaterialTheme.colorScheme.error) }
        Button(onClick = viewModel::save, enabled = !state.saving, modifier = Modifier.fillMaxWidth()) {
            Text(if (state.saving) "Saving…" else "Save")
        }
    }
}
