#pragma once

#include <optional>
#include <boost/json.hpp>

#include "auth.models.hpp"

namespace app::auth
{
    GoogleLoginRequest getGoogleLoginBodyFromJson(boost::json::value json_response);

    std::optional<boost::json::object> validateGoogleLoginBody(const GoogleLoginRequest &body);

}