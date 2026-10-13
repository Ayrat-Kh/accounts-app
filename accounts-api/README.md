# accounts-api

The backend for the receipts app. It does two things: it stores each receipt's **price** in PostgreSQL and its
**photo** in Garage (an S3-compatible object store). It also serves the model files that the phone downloads for
on-device scanning. The API contract is in [openapi.yaml](openapi.yaml). [docs/audit.md](docs/audit.md) explains
why the previous MongoDB/uWebSockets code was replaced.

## Reading order

Each folder has one responsibility. Reading top to bottom follows a request through the server:

| Folder | What it does |
|---|---|
| `src/main.cpp` | **Composition root.** Builds every object and wires the dependencies. Start here. |
| `src/http/` | `Server` is uWebSockets: it copies each request and runs the handler as a coroutine. `Router` maps "METHOD /path/:param" to handlers and checks authentication and body-size limits. `Response` holds the response builders and the single mapping from errors to HTTP status codes. `JsonFields` reads request bodies. |
| `src/auth/` | Google ID-token verification, Ed25519 access tokens, and rotating refresh tokens. |
| `src/receipts/` | The domain (`Receipt.hpp`), the SQL (`ReceiptRepository`), the use cases (`ReceiptService`), the wire format (`ReceiptJson`) and the HTTP routes (`ReceiptRoutes`). |
| `src/assets/` | Downloads model files in byte ranges. |
| `src/storage/` | The `ObjectStore` interface, with two implementations: Garage (`S3ObjectStore`) and an in-memory one for tests. |
| `src/db/` | Async libpq (`PgConnection`), the connection pool, and the migrations from `migrations/`. |
| `src/sync/` | The hook for syncing receipts into Obsidian later (see [docs/obsidian-sync.md](docs/obsidian-sync.md)). |
| `src/util/` | `Result`/`AppError`, time, crypto, files, and `BlockingExecutor`. |

### Ideas worth knowing

- **One thread, one event loop.** uWebSockets runs on the same `boost::asio::io_context` as every coroutine, because
  uSockets is built with its Asio backend (`vcpkg-overlays/usockets`). Postgres waits use the same loop: libpq runs
  in non-blocking mode and `co_await`s its socket. Since nothing runs in parallel, nothing needs a lock.
- **Blocking work runs elsewhere.** The AWS SDK only has blocking calls. `BlockingExecutor::run` executes them on a
  small thread pool and resumes the coroutine back on the loop.
- **Errors are values.** Functions return `Result<T>`, which is `std::expected<T, AppError>`.
  `http::errorResponse` turns an `AppError` into a status code and a JSON body; nothing else does that mapping.
- **Every query is scoped to its owner.** Every receipt query filters on `user_id`, and the user id only ever comes
  from the verified token. So a receipt that belongs to someone else returns 404, the same as one that doesn't exist.

## Build and test

Builds run in a Linux dev container (`accounts-api/docker/dev.Dockerfile`), never directly on macOS:

```bash
scripts/dev.sh scripts/bootstrap-vcpkg.sh
scripts/dev.sh cmake -S accounts-api --preset debug
scripts/dev.sh cmake --build accounts-api/build/debug
```

Unit tests run anywhere. The database and S3 tests skip themselves unless Postgres and Garage are up:

```bash
scripts/dev-up.sh
DEV_NETWORK=accounts-dev DEV_ENV_FILE=scripts/.dev-env scripts/dev.sh ctest --test-dir accounts-api/build/debug --output-on-failure
```

The `asan` preset builds the same code with AddressSanitizer and UBSan (CI runs both).

## Run locally

```bash
scripts/dev-up.sh
DEV_PORTS=1 DEV_NETWORK=accounts-dev scripts/dev.sh accounts-api/build/debug/accounts-api --config accounts-api/accounts.example.toml
curl localhost:8080/readyz
```

Production configuration and deployment are in [`../deploy`](../deploy/README.md).
