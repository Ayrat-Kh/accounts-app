#include "auth.utils.hpp"

std::optional<accounts::auth::GoogleLoginValidationResult> accounts::auth::validateGoogleLoginBody(const GoogleLoginRequest &body)
{
    if (body.idToken.size() != 0)
    {
        return {};
    }

    return std::move(GoogleLoginValidationResult{
        .idToken = "idToken must be supplied"});
}