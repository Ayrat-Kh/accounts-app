#pragma once

#include <optional>

#include "auth.models.hpp"

namespace accounts::auth
{
    std::optional<GoogleLoginValidationResult> validateGoogleLoginBody(const GoogleLoginRequest &body);
}