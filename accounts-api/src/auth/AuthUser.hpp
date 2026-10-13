#pragma once

#include <string>

namespace accounts {

// The identity carried by a verified access token.
struct AuthUser {
    std::string userId;  // users.id (uuid)
};

} // namespace accounts
