#pragma once

// Storage for refresh tokens. Callers pass SHA-256 hashes, never the tokens
// themselves (see util/Crypto.hpp).

#include "db/PgPool.hpp"
#include "util/Time.hpp"

#include <boost/asio/awaitable.hpp>

#include <optional>
#include <string>

namespace accounts::auth {

struct StoredRefreshToken {
    std::string userId;
    Timestamp absoluteExpiresAt; // carried over on rotation, so a session cannot live forever
};

class RefreshTokenRepository {
public:
    explicit RefreshTokenRepository(db::PgPool &pool) : pool_(pool) {}

    boost::asio::awaitable<Result<void>> insert(const std::string &userId, const std::string &tokenHash,
                                                Timestamp expiresAt, Timestamp absoluteExpiresAt);

    // Atomically revokes a valid (unrevoked, unexpired) token and returns who
    // it belonged to. A second call with the same hash finds nothing, so a
    // token can be exchanged exactly once.
    boost::asio::awaitable<Result<std::optional<StoredRefreshToken>>> consume(const std::string &tokenHash);

    boost::asio::awaitable<Result<void>> revoke(const std::string &tokenHash);

private:
    db::PgPool &pool_;
};

} // namespace accounts::auth
