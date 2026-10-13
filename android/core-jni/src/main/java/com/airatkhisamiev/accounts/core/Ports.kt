package com.airatkhisamiev.accounts.core

// What the C++ core needs from Android (see core/include/receipts/Ports.hpp).
// Native code calls these methods; keep their signatures in sync with KotlinPorts.cpp.

/** Small secrets (tokens) in protected storage. */
interface SecureStore {
    fun read(key: String): String?

    fun write(key: String, value: String)

    fun remove(key: String)
}

/** `level` follows receipts::LogLevel: 0 debug, 1 info, 2 warning, 3 error. */
fun interface CoreLogger {
    fun log(level: Int, message: String)
}

fun interface DownloadProgress {
    fun onProgress(bytesDone: Long, bytesTotal: Long)
}
