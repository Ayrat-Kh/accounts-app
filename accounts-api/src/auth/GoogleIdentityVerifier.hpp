#pragma once

// Verifies a Google ID token (from Android's Credential Manager) by asking
// Google's tokeninfo endpoint, then checks the claims that the old code
// skipped. Without the `aud` check, a token that Google issued to *any* app
// would log the user into ours.

#include "auth/IdentityVerifier.hpp"
#include "net/HttpsClient.hpp"

#include <boost/json/object.hpp>

#include <vector>

namespace accounts::auth {

class GoogleIdentityVerifier : public IdentityVerifier {
public:
    GoogleIdentityVerifier(net::HttpsClient &httpsClient, std::vector<std::string> allowedClientIds);

    boost::asio::awaitable<Result<VerifiedIdentity>> verify(std::string idToken) override;

private:
    net::HttpsClient &httpsClient_;
    std::vector<std::string> allowedClientIds_;
};

// The claim checks, separated from the network call so they can be unit-tested.
Result<VerifiedIdentity> checkGoogleTokenInfo(const boost::json::object &tokenInfo,
                                              const std::vector<std::string> &allowedClientIds);

} // namespace accounts::auth
