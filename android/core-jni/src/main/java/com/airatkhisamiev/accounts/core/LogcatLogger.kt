package com.airatkhisamiev.accounts.core

import android.util.Log

/** Sends the core's log lines to Logcat under the tag "ReceiptsCore". */
class LogcatLogger : CoreLogger {
    override fun log(level: Int, message: String) {
        when (level) {
            0 -> Log.d(TAG, message)
            1 -> Log.i(TAG, message)
            2 -> Log.w(TAG, message)
            else -> Log.e(TAG, message)
        }
    }

    private companion object {
        const val TAG = "ReceiptsCore"
    }
}
