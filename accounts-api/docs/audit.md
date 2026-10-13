# Backend audit (MongoDB / uWebSockets version, commit `512a14a`)

This audit covers the code that this rewrite replaced. Each finding names the old file, explains the risk, and says how the new code avoids it.

## Critical: security and memory safety

| # | Where (old) | Problem | Fix in the new code |
|---|-------------|---------|---------------------|
| 1 | `services/googleLogin.service.cpp` | The Google `tokeninfo` response was trusted without checking `aud`, `iss` or `email_verified`. An ID token that Google issued for **any** OAuth client could log in as that Google user. | `auth/GoogleIdentityVerifier.cpp` requires `aud ∈ google_client_ids`, a Google `iss`, `email_verified == true` and an unexpired `exp`. `AuthService` also checks the email against `allowed_emails`. |
| 2 | `services/sendHttpsRequest.hpp` | TLS used `set_default_verify_paths()` but never `set_verify_mode(verify_peer)` or host-name verification, so any certificate was accepted (MITM). | `net/HttpsClient.cpp` turns on `verify_peer` and `host_name_verification`, and sets SNI. |
| 3 | `utils/readRequestJson.hpp` | `RequestJsonBodyReader` was a stack local, yet its `onData` lambda captured `this`, which dangled once the handler returned (**use-after-free**). It also kept `uWS::HttpRequest*`, which is only valid during the synchronous handler call. | uWebSockets is replaced by a Boost.Beast server built on coroutines (`http/Server.cpp`). Each connection is one coroutine that owns its request and response objects, so nothing outlives its owner. |
| 4 | `utils/readRequestJson.hpp`, `geoCode/geoCode.handlers.cpp` | `onAborted` wrote a response to an already aborted connection. The geo handler called `abort()` without `return` and then ended the response a second time. A detached coroutine could write to `res` after the client had gone. | With one coroutine per connection there's no "aborted response" state to forget about. A failed write ends the coroutine through a `boost::system::error_code`. |
| 5 | `accounts/accounts.handlers.cpp` | **IDOR.** `PUT /v1/accounts/:id` checked only `body.userId == caller` and then upserted *by id*. A caller could overwrite another user's record, or create records under ids of their choosing. `POST` didn't check `userId` at all. | The client never sends `user_id`. It comes only from the verified access token, and every SQL statement in `receipts/ReceiptRepository.cpp` has `WHERE user_id = $1`. A row owned by someone else therefore returns 404. |
| 6 | `utils/readRequestJson.hpp` | The request body was appended without a limit (memory DoS). | Every route declares `max_body_bytes`. The parser enforces it before the body is read and answers `413`. |
| 7 | `services/jwt.service.cpp` | `jwtToken.remove_prefix(7)` on a header that could be shorter than 7 characters is undefined behaviour. | `auth/BearerToken.cpp` parses the header with explicit length and scheme checks, and has unit tests. |

## High: correctness and the coroutine model

- **Blocking calls on the event loop.** The Mongo driver calls were synchronous and ran on the only `io_context` thread, so every DB query stalled all other requests and the coroutines added nothing. `tcp::resolver::resolve` was blocking too.
  - New: libpq in non-blocking mode, driven by `co_await` on the socket (`db/PgConnection.cpp`), plus `async_resolve`.
  - The only blocking library left is the AWS SDK, which runs on a separate thread pool (`util/BlockingExecutor.hpp`).
- **`/v1/users/me/accounts` was always rejected.** The `userId != caller` check ran before the `me` alias was resolved.
- **Wrong user data.** `lastName` was filled from `given_name`.
- **Stubs and broken contracts.**
  - `getAccountById` was an empty stub.
  - `page`/`pageSize` were documented in the OpenAPI spec but ignored. New: keyset pagination with a `cursor`.
- **Crash on start in Kubernetes.** `ACCOUNTS_GEOCODE_API_KEY` was required but the k8s secret didn't set it. The geocoding feature is gone.

## Quality and best practices

- **Tokens.**
  - Old: HS256 with a shared secret, 7-day access tokens, no refresh and no way to revoke.
  - New: Ed25519-signed 15-minute access tokens, plus rotating refresh tokens that are stored hashed and can be revoked.
- **Money.** Stored as `double`. New: integer minor units (`amount_minor` = cents) and an ISO-4217 currency code.
- **Logging.**
  - Old: `std::cout` and `std::cerr`, and JWT errors could be printed.
  - New: `spdlog` with one JSON line per event. Tokens are never logged.
- **Dependencies and service location.** A global `AppDependencies` singleton served as the service locator. New: constructor injection, wired in one place (`main.cpp`).
- **Unreadable code.**
  - Large commented-out blocks in `main.cpp`.
  - `return std::move(x)` everywhere, which defeats NRVO.
  - A 345-line reflection-based BSON mapper, where explicit row-mapping functions are easier to read.
- **Missing operational basics.** No tests, no `/readyz`, no graceful shutdown on `SIGTERM`.
- **Build.**
  - `GLOB_RECURSE` sources.
  - Dependencies fetched at configure time with `FetchContent`/`ExternalProject` and floating branches.
  - New: a vcpkg manifest pinned to one baseline.
- **Docker.** The runtime image was a full Ubuntu. New: distroless and non-root.
