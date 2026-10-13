package com.airatkhisamiev.accounts.core

/** A failure reported by the C++ core (receipts::Error), thrown from native code. */
class CoreException(codeName: String, message: String) : Exception(message) {
    val code: Code = Code.entries.firstOrNull { it.name == codeName } ?: Code.INTERNAL

    enum class Code { INVALID_INPUT, NOT_SIGNED_IN, NETWORK, SERVER, NOT_FOUND, MODEL_MISSING, INTERNAL }
}
