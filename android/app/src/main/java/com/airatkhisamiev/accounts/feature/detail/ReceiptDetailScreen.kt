package com.airatkhisamiev.accounts.feature.detail

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.AlertDialog
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedButton
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import androidx.lifecycle.compose.collectAsStateWithLifecycle
import com.airatkhisamiev.accounts.core.SyncState
import com.airatkhisamiev.accounts.money.formatMoney
import com.airatkhisamiev.accounts.ui.common.ReceiptPhoto
import com.airatkhisamiev.accounts.ui.common.rfc3339Date

@Composable
fun ReceiptDetailScreen(viewModel: ReceiptDetailViewModel, onDeleted: () -> Unit) {
    val state by viewModel.state.collectAsStateWithLifecycle()
    var confirmDelete by remember { mutableStateOf(false) }
    LaunchedEffect(state.deleted) {
        if (state.deleted) onDeleted()
    }

    val receipt = state.receipt
    Column(
        modifier = Modifier.fillMaxSize().verticalScroll(rememberScrollState()).padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(12.dp)
    ) {
        if (receipt == null) {
            Text(state.error ?: "Loading…")
            return@Column
        }
        Text(formatMoney(receipt.money), style = MaterialTheme.typography.displaySmall)
        Text(receipt.merchant ?: "Unknown shop", style = MaterialTheme.typography.titleMedium)
        Text(rfc3339Date(receipt.purchasedAt))
        if (receipt.syncState != SyncState.SYNCED) {
            Text("Not uploaded yet", color = MaterialTheme.colorScheme.tertiary)
        }
        receipt.imagePath?.let { ReceiptPhoto(it, Modifier.fillMaxWidth()) }
        OutlinedButton(onClick = { confirmDelete = true }) { Text("Delete") }
        state.error?.let { Text(it, color = MaterialTheme.colorScheme.error) }
    }

    if (confirmDelete) {
        AlertDialog(
            onDismissRequest = { confirmDelete = false },
            title = { Text("Delete this receipt?") },
            text = { Text("It will also be removed from your other devices.") },
            confirmButton = {
                TextButton(onClick = {
                    confirmDelete = false
                    viewModel.delete()
                }) { Text("Delete") }
            },
            dismissButton = { TextButton(onClick = { confirmDelete = false }) { Text("Cancel") } }
        )
    }
}
