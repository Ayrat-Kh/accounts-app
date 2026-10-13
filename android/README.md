# Android app

The app is Jetpack Compose UI over the C++ core ([`../core`](../core)). Kotlin draws screens and talks to Android:
camera, Google sign-in, Keystore, WorkManager. Reading receipts, storing them and syncing them happens in C++.

## Modules

| Module | What it is |
|---|---|
| `:core-jni` | Builds `../core` with the NDK, plus the bindings. `src/main/cpp` is a thin JNI layer: values cross as JSON, errors as `CoreException`. `ReceiptsCore.kt` is the Kotlin API, with suspend functions on `Dispatchers.IO`. `KeystoreSecureStore` and `LogcatLogger` implement the core's ports. |
| `:app` | Screens. `AppContainer` is the composition root and `ui/AppNavigation.kt` is the map of all screens. Each `feature/*` has a `*ViewModel` (state as a `StateFlow`, calls into the core) and a `*Screen` (draws the state). |

How a scan flows through the app:

```
ScanScreen ──(photo, rotated upright)──► ReviewViewModel ──core.scanReceipt──► form pre-filled
   user checks / corrects ──► core.saveReceipt (offline first) ──► SyncScheduler ──► SyncWorker ──► core.sync
```

## Build

You need JDK 21, the Android SDK with platform 36, NDK `28.2.13676358` and CMake `3.31.6`. The first build also
compiles the core's dependencies (Tesseract, llama.cpp, curl, OpenSSL and more) for both ABIs with vcpkg, which
takes a while once and is cached afterwards.

The vcpkg Android triplets need the NDK path exported:

```bash
export ANDROID_NDK_HOME=$HOME/Library/Android/sdk/ndk/28.2.13676358
```

Bootstrap vcpkg (repository root), then build from `android/`:

```bash
scripts/bootstrap-vcpkg.sh
```

```bash
./gradlew ktlintCheck detekt testDebugUnitTest :app:assembleDebug
```

Settings live in `gradle.properties`:
- `accounts.googleServerClientId`: the OAuth **Web** client ID whose ID tokens accounts-api accepts.
- `accounts.apiBaseUrl`: the server.

Override either with `-P...`. On first launch the app asks you to sign in, then downloads the scanner models (about
350 MB) from the server.
