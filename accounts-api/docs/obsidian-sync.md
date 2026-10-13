# Syncing receipts into Obsidian (follow-up)

The goal is for every receipt to also show up as a note in the Obsidian vault served by obsync (`../obsidian`).

## What is already in place (this repo)

- `receipts.obsidian_synced_at` is set once a receipt has been published. Editing a receipt clears it again.
- `sync::ObsidianSink` is the interface that will publish one receipt. It must be idempotent.
- `sync::ObsidianSyncWorker` is a coroutine on the main loop. It periodically publishes unsynced receipts and then
  marks them. `main.cpp` doesn't start it yet, because there is no sink to give it.

## Why there is no sink yet

obsync stores every note as a Yjs CRDT document and accepts writes only over its WebSocket sync protocol, from
clients that signed in through Google OAuth. It has no API that a service can use to write a note. Writing directly
to its SQLite or Garage storage would corrupt live state, because the objects are encrypted CRDT updates and the
server keeps documents in memory.

## Proposed follow-up (in `../obsidian`)

1. **A service credential.** Add a `client_credentials`-style grant: a long-lived service key, stored as a
   Kubernetes Secret, is exchanged for a short-lived token that is scoped to one vault and one folder (e.g.
   `Finance/Receipts/`).
2. **A write endpoint.** For example `PUT /v1/vaults/:vault/notes?path=Finance/Receipts/2026-10/<receipt-id>.md`
   with a Markdown body. obsync applies it as a Yjs update through its document registry, so it reaches connected
   devices and is persisted the normal way. Using the receipt id as the file name makes repeated publishes
   idempotent.
3. **Here: `ObsyncSink`.** It implements `ObsidianSink` by rendering a note with front matter (`amount`,
   `currency`, `purchased_at`, `merchant`, `receipt_id`) and `PUT`ting it. `main.cpp` then starts the worker. The
   NetworkPolicy has to allow egress from `app-accounts` to `obsync.app-obsync:9000`.
