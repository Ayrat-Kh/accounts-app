#pragma once

// The one entry point a UI talks to. Every call is synchronous and
// thread-safe: hosts call it from a background thread (Kotlin:
// Dispatchers.IO, Swift: a Task) and never from the UI thread. The core
// itself starts no threads, so scheduling (e.g. "sync when back online")
// stays with the platform, which knows best.
//
// Offline first: saveReceipt() and deleteReceipt() change the local
// database immediately; sync() uploads pending changes and downloads the
// server's list when a connection is available.

#include "receipts/Config.hpp"
#include "receipts/Ports.hpp"
#include "receipts/Result.hpp"
#include "receipts/Types.hpp"

#include <memory>
#include <string>
#include <vector>

namespace receipts {

class ReceiptsService {
public:
    // Wires the concrete engines (Tesseract, llama.cpp or HTTP model servers,
    // libcurl, SQLite) according to `config`.
    static Result<std::unique_ptr<ReceiptsService>> create(CoreConfig config, std::shared_ptr<SecureStore> secureStore,
                                                           std::shared_ptr<Logger> logger);
    ~ReceiptsService();

    // --- Account ---
    Result<void> signInWithGoogle(const std::string &idToken);
    void signOut();
    bool isSignedIn() const;

    // --- Models (downloaded once from the server, ~350 MB) ---
    bool modelsReady() const;
    Result<void> downloadModels(ProgressListener *progress);

    // --- Scanning: photo -> suggested total, date and merchant ---
    Result<ScanResult> scanReceipt(const std::string &imagePath);

    // --- Receipts (local first) ---
    Result<Receipt> saveReceipt(const ReceiptDraft &draft);
    Result<std::vector<Receipt>> listReceipts();
    Result<void> deleteReceipt(const std::string &localId);

    // Uploads pending changes, then refreshes the local list from the server.
    Result<SyncReport> sync();

private:
    struct Impl;
    explicit ReceiptsService(std::unique_ptr<Impl> impl);
    std::unique_ptr<Impl> impl_;
};

} // namespace receipts
