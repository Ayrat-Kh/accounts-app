package com.airatkhisamiev.accounts.sync

import android.content.Context
import androidx.work.CoroutineWorker
import androidx.work.WorkerParameters
import com.airatkhisamiev.accounts.AccountsApplication
import com.airatkhisamiev.accounts.core.CoreException

/** Runs the core's sync in the background; WorkManager retries it when the network returns. */
class SyncWorker(context: Context, parameters: WorkerParameters) : CoroutineWorker(context, parameters) {
    override suspend fun doWork(): Result {
        val core = (applicationContext as AccountsApplication).container.core
        return try {
            core.sync()
            Result.success()
        } catch (error: CoreException) {
            when (error.code) {
                CoreException.Code.NETWORK, CoreException.Code.SERVER -> Result.retry()
                else -> Result.failure() // e.g. signed out: retrying cannot help
            }
        }
    }
}
