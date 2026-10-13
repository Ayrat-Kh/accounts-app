package com.airatkhisamiev.accounts.core

import kotlinx.coroutines.CoroutineDispatcher
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import kotlinx.serialization.json.Json

/**
 * The app's entry point to the C++ core. Every call is a suspend function
 * that runs the (blocking, thread-safe) native call on [dispatcher], so the
 * UI thread never waits on OCR, the model or the network. Failures surface
 * as [CoreException].
 */
@Suppress("TooManyFunctions") // one function per core operation: it mirrors ReceiptsService.hpp
class ReceiptsCore(
    config: CoreConfig,
    secureStore: SecureStore,
    logger: CoreLogger,
    private val dispatcher: CoroutineDispatcher = Dispatchers.IO
) : AutoCloseable {
    private val handle = NativeCore.create(json.encodeToString(config), secureStore, logger)

    suspend fun signInWithGoogle(idToken: String) = io { NativeCore.signInWithGoogle(handle, idToken) }

    suspend fun signOut() = io { NativeCore.signOut(handle) }

    suspend fun isSignedIn(): Boolean = io { NativeCore.isSignedIn(handle) }

    suspend fun modelsReady(): Boolean = io { NativeCore.modelsReady(handle) }

    suspend fun downloadModels(progress: DownloadProgress) = io { NativeCore.downloadModels(handle, progress) }

    suspend fun scanReceipt(imagePath: String): ScanResult = io { decode(NativeCore.scanReceipt(handle, imagePath)) }

    suspend fun saveReceipt(draft: ReceiptDraft): Receipt =
        io { decode(NativeCore.saveReceipt(handle, json.encodeToString(draft))) }

    suspend fun listReceipts(): List<Receipt> = io { decode(NativeCore.listReceipts(handle)) }

    suspend fun deleteReceipt(localId: String) = io { NativeCore.deleteReceipt(handle, localId) }

    suspend fun sync(): SyncReport = io { decode(NativeCore.sync(handle)) }

    override fun close() = NativeCore.destroy(handle)

    private suspend fun <T> io(block: () -> T): T = withContext(dispatcher) { block() }

    private inline fun <reified T> decode(text: String): T = json.decodeFromString(text)

    internal companion object {
        val json =
            Json {
                ignoreUnknownKeys = true
                encodeDefaults = true // CoreConfig's defaults (e.g. the CA directory) must reach C++
            }
    }
}
