# accounts-app

Photograph a receipt; the phone reads the total, you confirm it, and it is stored and synced. Everything that reads,
stores and syncs is portable C++. The apps only draw the UI.

```
 android/  Kotlin + Compose UI ──JNI──┐
 (macOS)   SwiftUI, planned ──────────┼──► core/  C++23: OCR (Tesseract) + EmbeddingGemma 2 (llama.cpp)
 receipts-cli ────────────────────────┘          local SQLite outbox, sync, API client
                                                    │ HTTPS
                                                    ▼
                                          accounts-api/  C++23: uWebSockets + coroutines
                                          PostgreSQL (prices) + Garage S3 (photos)
                                                    │
                                          deploy/  k3s on the fernfall VPS, next to ../obsidian
```

| Folder | Start with |
|---|---|
| [`accounts-api/`](accounts-api/README.md) | The backend. `docs/audit.md` explains what was wrong with the old one. |
| [`core/`](core/README.md) | All app logic, including how a total is picked from a photo. |
| [`android/`](android/README.md) | The Android app. |
| [`deploy/`](deploy/README.md) | First deployment and day-to-day operations. |
| `.github/workflows/` | CI. `backend` runs lint, tests, then a signed image and deploy. `core` runs Linux and macOS tests with the real models. `android` runs lint, tests and the APK build, and makes a release on `v*` tags. |
| `scripts/` | Dev container (`dev.sh`), local Postgres and Garage (`dev-up.sh`), models, vcpkg. |

Code conventions, applied everywhere:
- Small single-purpose classes, with dependencies passed through constructors and wired in one composition root per
  program.
- Errors are values (`std::expected`, `Result`).
- Comments explain *why*.
- Every dependency is pinned (vcpkg baseline and overlays, Gradle version catalog, image digests, action SHAs).
