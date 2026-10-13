package com.airatkhisamiev.accounts.feature.scan

import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.PickVisualMediaRequest
import androidx.activity.result.contract.ActivityResultContracts
import androidx.camera.core.ImageCapture
import androidx.camera.core.ImageCaptureException
import androidx.camera.view.LifecycleCameraController
import androidx.camera.view.PreviewView
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Button
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.FilledTonalButton
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.dp
import androidx.compose.ui.viewinterop.AndroidView
import androidx.core.content.ContextCompat
import androidx.lifecycle.compose.LocalLifecycleOwner
import com.airatkhisamiev.accounts.photo.PhotoFiles
import java.io.File
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext

/**
 * Takes a photo (CameraX) or picks one from the gallery, makes it upright,
 * and passes its path on. No ViewModel: there is no state worth keeping.
 */
@Composable
fun ScanScreen(onPhoto: (File) -> Unit) {
    val context = LocalContext.current
    val scope = rememberCoroutineScope()
    var preparing by remember { mutableStateOf(false) }
    var cameraAllowed by remember { mutableStateOf(hasCameraPermission(context)) }

    fun usePhoto(file: File) {
        preparing = true
        scope.launch {
            withContext(Dispatchers.IO) { PhotoFiles.makeUpright(file) }
            onPhoto(file)
        }
    }

    val askCamera =
        rememberLauncherForActivityResult(ActivityResultContracts.RequestPermission()) { cameraAllowed = it }
    val pickImage =
        rememberLauncherForActivityResult(ActivityResultContracts.PickVisualMedia()) { uri ->
            if (uri != null) {
                scope.launch { usePhoto(withContext(Dispatchers.IO) { PhotoFiles.copyFromUri(context, uri) }) }
            }
        }
    LaunchedEffect(Unit) {
        if (!cameraAllowed) askCamera.launch(Manifest.permission.CAMERA)
    }

    val lifecycleOwner = LocalLifecycleOwner.current
    val camera = remember { LifecycleCameraController(context) }

    Box(Modifier.fillMaxSize()) {
        if (cameraAllowed) {
            AndroidView(
                factory = { viewContext ->
                    camera.bindToLifecycle(lifecycleOwner)
                    PreviewView(viewContext).apply { controller = camera }
                },
                modifier = Modifier.fillMaxSize()
            )
        }
        if (preparing) {
            CircularProgressIndicator(Modifier.align(Alignment.Center))
        }
        Row(
            modifier = Modifier.align(Alignment.BottomCenter).fillMaxWidth().padding(24.dp),
            horizontalArrangement = Arrangement.SpaceEvenly
        ) {
            FilledTonalButton(onClick = {
                pickImage.launch(PickVisualMediaRequest(ActivityResultContracts.PickVisualMedia.ImageOnly))
            }) { Text("Gallery") }
            Button(enabled = cameraAllowed && !preparing, onClick = {
                val file = PhotoFiles.newPhotoFile(context)
                camera.takePicture(
                    ImageCapture.OutputFileOptions.Builder(file).build(),
                    ContextCompat.getMainExecutor(context),
                    object : ImageCapture.OnImageSavedCallback {
                        override fun onImageSaved(output: ImageCapture.OutputFileResults) = usePhoto(file)

                        override fun onError(exception: ImageCaptureException) {
                            preparing = false
                        }
                    }
                )
            }) { Text("Take photo") }
        }
    }
}

private fun hasCameraPermission(context: Context) =
    ContextCompat.checkSelfPermission(context, Manifest.permission.CAMERA) == PackageManager.PERMISSION_GRANTED
