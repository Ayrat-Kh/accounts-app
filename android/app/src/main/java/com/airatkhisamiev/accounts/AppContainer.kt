package com.airatkhisamiev.accounts

import android.content.Context
import com.airatkhisamiev.accounts.core.CoreConfig
import com.airatkhisamiev.accounts.core.KeystoreSecureStore
import com.airatkhisamiev.accounts.core.LogcatLogger
import com.airatkhisamiev.accounts.core.ReceiptsCore
import com.airatkhisamiev.accounts.settings.UserSettings
import com.airatkhisamiev.accounts.sync.SyncScheduler

/**
 * The app's composition root: the one place that creates long-lived objects
 * and hands them to screens. Everything else receives what it needs.
 */
class AppContainer(context: Context) {
    val settings = UserSettings(context)

    val core =
        ReceiptsCore(
            config =
            CoreConfig(
                apiBaseUrl = BuildConfig.API_BASE_URL,
                dataDirectory = context.filesDir.resolve("receipts").absolutePath,
                defaultCurrency = settings.defaultCurrency
            ),
            secureStore = KeystoreSecureStore(context),
            logger = LogcatLogger()
        )

    val syncScheduler = SyncScheduler(context)
}
