#include "auth/RefreshTokenRepository.hpp"

namespace accounts::auth {

using boost::asio::awaitable;

awaitable<Result<void>> RefreshTokenRepository::insert(const std::string &userId, const std::string &tokenHash,
                                                       Timestamp expiresAt, Timestamp absoluteExpiresAt) {
    return pool_.command(R"sql(
        INSERT INTO refresh_tokens (user_id, token_hash, expires_at, absolute_expires_at)
        VALUES ($1, $2, $3, $4)
    )sql",
                         userId, tokenHash, expiresAt, absoluteExpiresAt);
}

awaitable<Result<std::optional<StoredRefreshToken>>> RefreshTokenRepository::consume(const std::string &tokenHash) {
    // UPDATE ... RETURNING is a single atomic statement: two concurrent
    // refreshes with the same token cannot both succeed.
    auto result = co_await pool_.query(R"sql(
        UPDATE refresh_tokens SET revoked_at = now()
        WHERE token_hash = $1
          AND revoked_at IS NULL
          AND expires_at > now()
          AND absolute_expires_at > now()
        RETURNING user_id, rfc3339(absolute_expires_at) AS absolute_expires_at
    )sql",
                                       tokenHash);
    if (!result) {
        co_return std::unexpected(result.error());
    }
    if (result->empty()) {
        co_return std::nullopt;
    }
    const db::PgRow row = result->firstRow();
    co_return StoredRefreshToken{row.text("user_id"), parseRfc3339(row.text("absolute_expires_at")).value()};
}

awaitable<Result<void>> RefreshTokenRepository::revoke(const std::string &tokenHash) {
    return pool_.command("UPDATE refresh_tokens SET revoked_at = now() WHERE token_hash = $1 AND revoked_at IS NULL",
                         tokenHash);
}

} // namespace accounts::auth
