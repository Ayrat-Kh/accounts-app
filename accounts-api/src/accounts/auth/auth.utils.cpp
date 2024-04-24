#include "auth.utils.hpp"

using namespace accounts;

std::optional<GoogleLoginValidationResult> accounts::validateGoogleLoginBody(const GoogleLoginRequest &body)
{
    if (body.idToken.size() != 0)
    {
        return {};
    }

    return std::move(GoogleLoginValidationResult{
        .idToken = "idToken must be supplied"});
}