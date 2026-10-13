package com.airatkhisamiev.accounts.core

/**
 * The raw native methods (ReceiptsCoreJni.cpp). Blocking; values are JSON
 * strings. Only [ReceiptsCore] uses this.
 */
@Suppress("TooManyFunctions") // one external function per native method in ReceiptsCoreJni.cpp
internal object NativeCore {
    init {
        System.loadLibrary("receipts_jni")
    }

    external fun create(configJson: String, secureStore: SecureStore, logger: CoreLogger): Long

    external fun destroy(handle: Long)

    external fun signInWithGoogle(handle: Long, idToken: String)

    external fun signOut(handle: Long)

    external fun isSignedIn(handle: Long): Boolean

    external fun modelsReady(handle: Long): Boolean

    external fun downloadModels(handle: Long, progress: DownloadProgress)

    external fun scanReceipt(handle: Long, imagePath: String): String

    external fun saveReceipt(handle: Long, draftJson: String): String

    external fun listReceipts(handle: Long): String

    external fun deleteReceipt(handle: Long, localId: String)

    external fun sync(handle: Long): String
}
