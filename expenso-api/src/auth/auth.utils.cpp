#include "auth.utils.hpp"

app::auth::GoogleLoginRequest app::auth::getGoogleLoginBodyFromJson(boost::json::value json_response)
{
    GoogleLoginRequest result;

    if (json_response.is_object())
    {
        auto idToken = json_response.as_object().if_contains("idToken");

        if (idToken != nullptr && idToken->kind() == boost::json::kind::string)
        {
            result.idToken = idToken->as_string();
        }
    }

    return std::move(result);
}

std::optional<boost::json::object> app::auth::validateGoogleLoginBody(const GoogleLoginRequest &body)
{
    if (body.idToken.size() != 0)
    {
        return {};
    }

    return std::move(boost::json::object({{"idToken", "idToken must be supplied"}}));
}