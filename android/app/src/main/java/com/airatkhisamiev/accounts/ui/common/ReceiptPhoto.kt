package com.airatkhisamiev.accounts.ui.common

import android.graphics.BitmapFactory
import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.heightIn
import androidx.compose.runtime.Composable
import androidx.compose.runtime.produceState
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.ImageBitmap
import androidx.compose.ui.graphics.asImageBitmap
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.unit.dp
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext

private const val PREVIEW_SAMPLE_SIZE = 4 // a 4000 px photo becomes 1000 px: plenty for a preview

/** Shows a local photo, decoded small and off the main thread. */
@Composable
fun ReceiptPhoto(path: String, modifier: Modifier = Modifier) {
    val bitmap =
        produceState<ImageBitmap?>(null, path) {
            value =
                withContext(Dispatchers.IO) {
                    BitmapFactory
                        .decodeFile(path, BitmapFactory.Options().apply { inSampleSize = PREVIEW_SAMPLE_SIZE })
                        ?.asImageBitmap()
                }
        }.value ?: return
    Image(
        bitmap,
        contentDescription = "Receipt photo",
        modifier.heightIn(max = 320.dp),
        contentScale = ContentScale.Fit
    )
}
