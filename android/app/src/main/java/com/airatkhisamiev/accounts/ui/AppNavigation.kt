package com.airatkhisamiev.accounts.ui

import android.net.Uri
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.lifecycle.viewmodel.compose.viewModel
import androidx.navigation.NavHostController
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable
import androidx.navigation.compose.rememberNavController
import com.airatkhisamiev.accounts.AppContainer
import com.airatkhisamiev.accounts.feature.detail.ReceiptDetailScreen
import com.airatkhisamiev.accounts.feature.detail.ReceiptDetailViewModel
import com.airatkhisamiev.accounts.feature.models.ModelsScreen
import com.airatkhisamiev.accounts.feature.models.ModelsViewModel
import com.airatkhisamiev.accounts.feature.receipts.ReceiptListScreen
import com.airatkhisamiev.accounts.feature.receipts.ReceiptListViewModel
import com.airatkhisamiev.accounts.feature.review.ReviewScreen
import com.airatkhisamiev.accounts.feature.review.ReviewViewModel
import com.airatkhisamiev.accounts.feature.scan.ScanScreen
import com.airatkhisamiev.accounts.feature.settings.SettingsScreen
import com.airatkhisamiev.accounts.feature.settings.SettingsViewModel
import com.airatkhisamiev.accounts.feature.signin.SignInScreen
import com.airatkhisamiev.accounts.feature.signin.SignInViewModel

/** Every screen of the app, and how they connect. */
private object Routes {
    const val SIGN_IN = "sign-in"
    const val MODELS = "models"
    const val RECEIPTS = "receipts"
    const val SCAN = "scan"
    const val REVIEW = "review/{photo}"
    const val DETAIL = "detail/{localId}"
    const val SETTINGS = "settings"

    fun review(photoPath: String) = "review/${Uri.encode(photoPath)}"

    fun detail(localId: String) = "detail/$localId"
}

@Composable
fun AppNavigation(container: AppContainer) {
    // Where to start depends on the core's state: signed in? models downloaded?
    var start by remember { mutableStateOf<String?>(null) }
    LaunchedEffect(Unit) {
        start =
            when {
                !container.core.isSignedIn() -> Routes.SIGN_IN
                !container.core.modelsReady() -> Routes.MODELS
                else -> Routes.RECEIPTS.also { container.syncScheduler.requestSync() }
            }
    }
    val startDestination =
        start ?: return Box(Modifier.fillMaxSize(), contentAlignment = Alignment.Center) { CircularProgressIndicator() }

    val navController = rememberNavController()
    NavHost(navController, startDestination = startDestination) {
        composable(Routes.SIGN_IN) {
            SignInScreen(viewModel { SignInViewModel(container.core) }) {
                navController.replaceAllWith(Routes.MODELS)
            }
        }
        composable(Routes.MODELS) {
            ModelsScreen(viewModel { ModelsViewModel(container.core) }) {
                container.syncScheduler.requestSync()
                navController.replaceAllWith(Routes.RECEIPTS)
            }
        }
        composable(Routes.RECEIPTS) {
            ReceiptListScreen(
                viewModel = viewModel { ReceiptListViewModel(container.core) },
                onScan = { navController.navigate(Routes.SCAN) },
                onOpen = { navController.navigate(Routes.detail(it.localId)) },
                onSettings = { navController.navigate(Routes.SETTINGS) }
            )
        }
        composable(Routes.SCAN) {
            ScanScreen { photo ->
                navController.navigate(Routes.review(photo.absolutePath)) { popUpTo(Routes.RECEIPTS) }
            }
        }
        composable(Routes.REVIEW) { entry ->
            val photoPath = requireNotNull(entry.arguments?.getString("photo"))
            ReviewScreen(
                viewModel = viewModel {
                    ReviewViewModel(
                        container.core,
                        photoPath,
                        container.syncScheduler,
                        container.settings.defaultCurrency
                    )
                },
                photoPath = photoPath,
                onSaved = { navController.popBackStack(Routes.RECEIPTS, inclusive = false) }
            )
        }
        composable(Routes.DETAIL) { entry ->
            val localId = requireNotNull(entry.arguments?.getString("localId"))
            ReceiptDetailScreen(
                viewModel {
                    ReceiptDetailViewModel(container.core, container.syncScheduler, localId)
                }
            ) {
                navController.popBackStack()
            }
        }
        composable(Routes.SETTINGS) {
            SettingsScreen(viewModel { SettingsViewModel(container.core, container.settings) }) {
                navController.replaceAllWith(Routes.SIGN_IN)
            }
        }
    }
}

/** Navigates to [route] and forgets the history, so Back doesn't return to sign-in or setup. */
private fun NavHostController.replaceAllWith(route: String) = navigate(route) { popUpTo(graph.id) { inclusive = true } }
