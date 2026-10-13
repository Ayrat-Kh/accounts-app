# receipts-core

All the logic of the receipts app, in portable C++23. The app shells only draw the UI:
- **Android:** Kotlin + Compose, through JNI ([`../android`](../android)).
- **macOS:** SwiftUI through C++ interop (planned), using `scripts/build-xcframework.sh`.
- **Terminal:** `receipts-cli`.

The core never includes a platform header.

## What it does

```
photo ──► OCR (Tesseract) ──► lines ──► PriceExtractor ──► ScanResult {total, date, merchant, confidence}
                                           │
                                           ├─ LineScorer: "does this line read like the total?"
                                           │    EmbeddingLineScorer  EmbeddingGemma 2, on the device (llama.cpp)
                                           │                         or on any OpenAI-compatible server
                                           │    KeywordLineScorer    no model: literal keywords (fallback)
                                           └─ LlmTotalExtractor (optional): a generative model's second opinion
                                                when unsure; only accepted if the amount is printed on the receipt

save ──► LocalReceiptStore (SQLite, offline first) ──► SyncEngine ──► ApiClient ──► accounts-api
```

## Reading order

| Folder | What it does |
|---|---|
| `include/receipts/` | **The public API.** `ReceiptsService` is the facade every UI calls. `Types`, `Config`, `Result` and `Ports` define the platform interfaces: `SecureStore`, `Logger`, `ProgressListener`. |
| `src/app/` | `ReceiptsService.cpp` is the composition root that wires everything. `SyncEngine` handles outbox upload and server pull. |
| `src/extract/` | Price extraction. `AmountParser` finds prices, `ReceiptFields` finds the date and merchant, `TotalAnchors` lists what totals and non-totals look like, `*LineScorer` classifies lines, and `PriceExtractor` combines the scores (the weights are explained in its header). |
| `src/ocr/` | The `OcrEngine` interface and Tesseract, including the image preparation that matters for photos. |
| `src/inference/` | The `EmbeddingEngine` and `CompletionEngine` interfaces, with in-process llama.cpp and OpenAI-compatible HTTP implementations. |
| `src/api/`, `src/net/` | `ApiClient` handles token refresh and pagination. `HttpTransport` has a libcurl implementation and fakes for the tests. |
| `src/store/` | A SQLite RAII wrapper and the local receipt store. |
| `src/models/` | Resumable, verified downloads of the model files from the server. |

Design rules used throughout:
- **Dependencies are interfaces, injected through constructors.** Tests swap in fakes for the network, the model
  server and the OCR.
- **Errors are values.** `Result<T>` = `std::expected`; no exception crosses the public API.
- **The core starts no threads.** Every call is synchronous and thread-safe, and hosts call it from their own
  background threads.

## Build and test (macOS or Linux)

```bash
scripts/bootstrap-vcpkg.sh
cmake -S core --preset debug
cmake --build core/build/debug
core/build/debug/receipts_core_tests
```

`ScanAccuracyTest` runs real OCR and EmbeddingGemma on the synthetic receipts in `testdata/receipts` (made by
`testdata/generate_receipts.py`). It needs the models and is skipped without them:

```bash
scripts/download-models.sh core/testdata/models
```

On that set both scorers find all 16 totals. What the embedding adds is confidence: it recognises wording the
keyword list lacks ("Betrag", "Amount payable"), so the UI knows when to ask the user.

## Trying it on a Mac

Each of these prints the scan result as JSON. In-process (Metal):

```bash
core/build/debug/receipts-cli scan photo.jpg
```

Against a model server: llama.cpp's `llama-server --embeddings -m embeddinggemma-2-Q8_0.gguf --port 8080`, oMLX, or
Ollama:

```bash
core/build/debug/receipts-cli scan photo.jpg --engine remote --base-url http://localhost:8080 --model embeddinggemma-2
```

Add a bigger generative model's second opinion for unclear receipts (any OpenAI-compatible
`/v1/chat/completions`):

```bash
core/build/debug/receipts-cli scan photo.jpg --completion-url http://localhost:8000 --completion-model gemma-4-12b
```

In the app, the same choices are `CoreConfig::embedding` / `CoreConfig::completion` with
`EngineConfig::Kind::OpenAiCompatible`.

## Model assets

Phones download the models from accounts-api (`/v1/assets`), not from Hugging Face. Publishing them is in
[`deploy/README.md`](../deploy/README.md). The files:
- `eng`, `deu` and `rus` `.traineddata` from tessdata_best
- `embeddinggemma-2-Q8_0.gguf`, Apache 2.0, about 310 MB

`scripts/download-models.sh` pins all of them to exact upstream revisions. llama.cpp itself is pinned through
`vcpkg-overlays/llama-cpp`, because EmbeddingGemma 2 needs build b11446 or newer.
