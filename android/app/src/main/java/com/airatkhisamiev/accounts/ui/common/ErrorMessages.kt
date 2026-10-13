package com.airatkhisamiev.accounts.ui.common

import com.airatkhisamiev.accounts.core.CoreException

/** A sentence for the user, for any failure the screens can run into. */
fun Throwable.userMessage(): String = when ((this as? CoreException)?.code) {
    CoreException.Code.NETWORK -> "No connection. Your changes are saved and will sync later."
    CoreException.Code.NOT_SIGNED_IN -> "Please sign in again."
    CoreException.Code.MODEL_MISSING -> "The scanner models are not downloaded yet."
    CoreException.Code.INVALID_INPUT -> message ?: "Please check your input."
    else -> message ?: "Something went wrong."
}
