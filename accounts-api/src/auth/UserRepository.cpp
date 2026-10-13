#include "auth/UserRepository.hpp"

namespace accounts::auth {

using boost::asio::awaitable;

awaitable<Result<User>> UserRepository::upsertByGoogleSubject(const std::string &googleSubject,
                                                              const std::string &email) {
    auto result = co_await pool_.query(R"sql(
        INSERT INTO users (google_sub, email) VALUES ($1, $2)
        ON CONFLICT (google_sub) DO UPDATE SET email = EXCLUDED.email
        RETURNING id, email
    )sql",
                                       googleSubject, email);
    if (!result) {
        co_return std::unexpected(result.error());
    }
    const db::PgRow row = result->firstRow();
    co_return User{row.text("id"), row.text("email")};
}

} // namespace accounts::auth
