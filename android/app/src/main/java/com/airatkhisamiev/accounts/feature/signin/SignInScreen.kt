package com.airatkhisamiev.accounts.feature.signin

import android.content.Context
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Button
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.dp
import androidx.credentials.CredentialManager
import androidx.credentials.CustomCredential
import androidx.credentials.GetCredentialRequest
import androidx.credentials.exceptions.GetCredentialException
import androidx.lifecycle.compose.collectAsStateWithLifecycle
import com.airatkhisamiev.accounts.BuildConfig
import com.google.android.libraries.identity.googleid.GetSignInWithGoogleOption
import com.google.android.libraries.identity.googleid.GoogleIdTokenCredential
import kotlinx.coroutines.launch

@Composable
fun SignInScreen(viewModel: SignInViewModel, onSignedIn: () -> Unit) {
    val state by viewModel.state.collectAsStateWithLifecycle()
    val context = LocalContext.current
    val scope = rememberCoroutineScope()

    LaunchedEffect(state.signedIn) {
        if (state.signedIn) onSignedIn()
    }

    Column(
        modifier = Modifier.fillMaxSize().padding(32.dp),
        verticalArrangement = Arrangement.spacedBy(24.dp, Alignment.CenterVertically),
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        Text("Receipts", style = MaterialTheme.typography.displaySmall)
        Text("Scan receipts and keep track of what you spend.", style = MaterialTheme.typography.bodyLarge)
        if (state.busy) {
            CircularProgressIndicator()
        } else {
            Button(onClick = {
                scope.launch {
                    requestGoogleIdToken(context)
                        .onSuccess(viewModel::signIn)
                        .onFailure { viewModel.showError(it.message ?: "Google sign-in failed") }
                }
            }) { Text("Sign in with Google") }
        }
        state.error?.let { Text(it, color = MaterialTheme.colorScheme.error) }
    }
}

/** Shows Google's account picker and returns the chosen account's ID token. */
private suspend fun requestGoogleIdToken(context: Context): Result<String> = try {
    val request =
        GetCredentialRequest
            .Builder()
            .addCredentialOption(GetSignInWithGoogleOption.Builder(BuildConfig.GOOGLE_SERVER_CLIENT_ID).build())
            .build()
    val credential = CredentialManager.create(context).getCredential(context, request).credential
    if (credential is CustomCredential &&
        credential.type == GoogleIdTokenCredential.TYPE_GOOGLE_ID_TOKEN_CREDENTIAL
    ) {
        Result.success(GoogleIdTokenCredential.createFrom(credential.data).idToken)
    } else {
        Result.failure(IllegalStateException("Unexpected credential type"))
    }
} catch (error: GetCredentialException) {
    Result.failure(error)
}
