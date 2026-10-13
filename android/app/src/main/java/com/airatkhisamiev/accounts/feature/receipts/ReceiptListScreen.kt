package com.airatkhisamiev.accounts.feature.receipts

import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.CameraAlt
import androidx.compose.material.icons.filled.CloudUpload
import androidx.compose.material.icons.filled.Settings
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.ExtendedFloatingActionButton
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.ListItem
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.material3.TopAppBar
import androidx.compose.material3.pulltorefresh.PullToRefreshBox
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import androidx.lifecycle.compose.collectAsStateWithLifecycle
import com.airatkhisamiev.accounts.core.Receipt
import com.airatkhisamiev.accounts.core.SyncState
import com.airatkhisamiev.accounts.money.formatMoney
import com.airatkhisamiev.accounts.ui.common.rfc3339Date

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun ReceiptListScreen(
    viewModel: ReceiptListViewModel,
    onScan: () -> Unit,
    onOpen: (Receipt) -> Unit,
    onSettings: () -> Unit
) {
    val state by viewModel.state.collectAsStateWithLifecycle()
    LaunchedEffect(Unit) { viewModel.load() }

    Scaffold(
        topBar = {
            TopAppBar(
                title = { Text("Receipts") },
                actions = {
                    IconButton(onClick = onSettings) { Icon(Icons.Default.Settings, contentDescription = "Settings") }
                }
            )
        },
        floatingActionButton = {
            ExtendedFloatingActionButton(
                onClick = onScan,
                icon = { Icon(Icons.Default.CameraAlt, contentDescription = null) },
                text = { Text("Scan") }
            )
        }
    ) { padding ->
        PullToRefreshBox(
            isRefreshing = state.refreshing,
            onRefresh = viewModel::refresh,
            modifier = Modifier.padding(padding).fillMaxSize()
        ) {
            if (state.months.isEmpty()) {
                Box(Modifier.fillMaxSize(), contentAlignment = Alignment.Center) {
                    Text(state.message ?: "No receipts yet. Tap Scan to add one.")
                }
            } else {
                LazyColumn(Modifier.fillMaxSize()) {
                    state.message?.let { message ->
                        item { Text(message, Modifier.padding(16.dp), color = MaterialTheme.colorScheme.error) }
                    }
                    state.months.forEach { month ->
                        item(key = month.month) { MonthHeader(month) }
                        items(month.receipts, key = { it.localId }) { receipt -> ReceiptRow(receipt, onOpen) }
                    }
                }
            }
        }
    }
}

@Composable
private fun MonthHeader(month: ReceiptListViewModel.MonthGroup) {
    Row(
        modifier = Modifier.fillMaxWidth().padding(horizontal = 16.dp, vertical = 12.dp),
        horizontalArrangement = Arrangement.SpaceBetween
    ) {
        Text(month.month, style = MaterialTheme.typography.titleMedium)
        Text(month.totals.joinToString(" + ") { formatMoney(it) }, style = MaterialTheme.typography.titleMedium)
    }
    HorizontalDivider()
}

@Composable
private fun ReceiptRow(receipt: Receipt, onOpen: (Receipt) -> Unit) {
    ListItem(
        modifier = Modifier.clickable { onOpen(receipt) },
        headlineContent = { Text(receipt.merchant ?: "Receipt") },
        supportingContent = { Text(rfc3339Date(receipt.purchasedAt)) },
        trailingContent = {
            Row(verticalAlignment = Alignment.CenterVertically) {
                if (receipt.syncState != SyncState.SYNCED) {
                    Icon(Icons.Default.CloudUpload, contentDescription = "Not synced yet")
                }
                Text(formatMoney(receipt.money), Modifier.padding(start = 8.dp))
            }
        }
    )
}
