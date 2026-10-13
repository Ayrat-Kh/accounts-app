package com.airatkhisamiev.accounts.photo

import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.graphics.Matrix
import android.net.Uri
import androidx.exifinterface.media.ExifInterface
import java.io.File

/**
 * Photo files handed to the core. Cameras store rotation as an EXIF tag
 * instead of rotating pixels, and Tesseract ignores EXIF, so every photo is
 * rewritten upright before it is scanned.
 */
object PhotoFiles {
    fun newPhotoFile(context: Context): File =
        File(context.cacheDir, "photos").apply { mkdirs() }.resolve("receipt-${System.currentTimeMillis()}.jpg")

    /** Copies a gallery image into our cache, so the core can read it as a file. */
    fun copyFromUri(context: Context, uri: Uri): File {
        val target = newPhotoFile(context)
        context.contentResolver.openInputStream(uri).use { input ->
            requireNotNull(input) { "cannot open $uri" }
            target.outputStream().use { input.copyTo(it) }
        }
        return target
    }

    /** Rotates the image's pixels to match its EXIF orientation, in place. */
    fun makeUpright(file: File) {
        val degrees = ExifInterface(file.absolutePath).rotationDegrees
        if (degrees == 0) {
            return
        }
        val original = BitmapFactory.decodeFile(file.absolutePath) ?: return
        val rotated =
            Bitmap.createBitmap(
                original,
                0,
                0,
                original.width,
                original.height,
                Matrix().apply { postRotate(degrees.toFloat()) },
                true
            )
        file.outputStream().use { rotated.compress(Bitmap.CompressFormat.JPEG, JPEG_QUALITY, it) }
        original.recycle()
        rotated.recycle()
    }

    private const val JPEG_QUALITY = 92
}
