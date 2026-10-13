package com.airatkhisamiev.accounts.feature.models

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Button
import androidx.compose.material3.LinearProgressIndicator
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import androidx.lifecycle.compose.collectAsStateWithLifecycle

private const val BYTES_PER_MB = 1_048_576

@Composable
fun ModelsScreen(viewModel: ModelsViewModel, onReady: () -> Unit) {
    val state by viewModel.state.collectAsStateWithLifecycle()
    LaunchedEffect(state.ready) {
        if (state.ready) onReady()
    }

    Column(
        modifier = Modifier.fillMaxSize().padding(32.dp),
        verticalArrangement = Arrangement.spacedBy(16.dp, Alignment.CenterVertically),
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        Text("One-time setup", style = MaterialTheme.typography.headlineSmall)
        Text(
            "Receipts are read on your phone, so the text recognition and the language model " +
                "(about 350 MB) are downloaded once. Wi-Fi recommended."
        )
        if (state.downloading) {
            LinearProgressIndicator(progress = { state.progress }, modifier = Modifier.fillMaxWidth())
            Text("${state.bytesDone / BYTES_PER_MB} / ${state.bytesTotal / BYTES_PER_MB} MB")
        } else {
            Button(onClick = viewModel::download) { Text(if (state.error == null) "Download" else "Try again") }
        }
        state.error?.let { Text(it, color = MaterialTheme.colorScheme.error) }
    }
}
