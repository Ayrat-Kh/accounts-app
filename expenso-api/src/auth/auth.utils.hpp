#pragma once

#include <optional>

#include "auth.models.hpp"

namespace app::auth
{
    std::optional<GoogleLoginValidationResult> validateGoogleLoginBody(const GoogleLoginRequest &body);
}