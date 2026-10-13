#pragma once

#include "db/PgPool.hpp"

#include <boost/asio/awaitable.hpp>

#include <string>

namespace accounts::auth {

struct User {
    std::string id;
    std::string email;
};

class UserRepository {
public:
    explicit UserRepository(db::PgPool &pool) : pool_(pool) {}

    // Returns the user with this Google subject, creating it on first sign-in.
    // The e-mail is refreshed on every sign-in, since users can change it at Google.
    boost::asio::awaitable<Result<User>> upsertByGoogleSubject(const std::string &googleSubject,
                                                               const std::string &email);

private:
    db::PgPool &pool_;
};

} // namespace accounts::auth
