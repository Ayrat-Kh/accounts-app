#pragma once

// "Who does this third-party sign-in token belong to?" Implemented by
// GoogleIdentityVerifier; tests substitute a fake.

#include "util/Error.hpp"

#include <boost/asio/awaitable.hpp>

#include <string>

namespace accounts::auth {

struct VerifiedIdentity {
    std::string subject;  // the provider's stable user id ("sub")
    std::string email;    // verified e-mail address
};

class IdentityVerifier {
public:
    virtual ~IdentityVerifier() = default;
    virtual boost::asio::awaitable<Result<VerifiedIdentity>> verify(std::string idToken) = 0;
};

} // namespace accounts::auth
