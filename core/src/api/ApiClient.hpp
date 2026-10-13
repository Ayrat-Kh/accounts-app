#pragma once

// Client for accounts-api (see accounts-api/openapi.yaml).
//
// Authentication is handled here and nowhere else: every call carries the
// access token, and a 401 triggers one refresh-and-retry. Tokens live in
// the host's SecureStore, so a restarted app stays signed in.

#include "net/HttpTransport.hpp"
#include "receipts/Ports.hpp"
#include "receipts/Types.hpp"

#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace receipts::api {

struct AssetInfo {
    std::string name;
    std::uint64_t size = 0;
    std::string sha256;
};

struct AssetPiece {
    std::string bytes;
    std::uint64_t totalSize = 0;
};

class ApiClient {
public:
    ApiClient(std::shared_ptr<net::HttpTransport> transport, std::string baseUrl,
              std::shared_ptr<SecureStore> secureStore);

    Result<void> signInWithGoogle(const std::string &idToken);
    void signOut(); // best effort on the server; always forgets the tokens locally
    bool isSignedIn() const;

    // Returned receipts have serverId set and no localId.
    Result<Receipt> createReceipt(const Receipt &receipt);
    Result<void> uploadImage(const std::string &serverId, const std::string &bytes);
    Result<void> deleteReceipt(const std::string &serverId); // already gone counts as success
    Result<std::vector<Receipt>> listAllReceipts();          // follows next_cursor to the end

    Result<std::vector<AssetInfo>> listAssets();
    // Bytes [first, last] of an asset (at most 8 MiB per call).
    Result<AssetPiece> downloadAssetPiece(const std::string &name, std::uint64_t first, std::uint64_t last);

private:
    Result<net::HttpResponse> send(net::HttpRequest request, bool authenticated = true);
    Result<net::HttpResponse> sendOnce(const net::HttpRequest &request, bool authenticated);
    Result<void> refreshTokens();
    Result<void> storeTokens(const net::HttpResponse &response);

    std::shared_ptr<net::HttpTransport> transport_;
    std::string baseUrl_;
    std::shared_ptr<SecureStore> secureStore_;
    std::mutex refreshMutex_; // two threads hitting 401 at once must not both rotate the token
};

// Maps a non-2xx response to an Error ({"error": {"message"}} if present).
Error errorFromResponse(const net::HttpResponse &response);

} // namespace receipts::api
