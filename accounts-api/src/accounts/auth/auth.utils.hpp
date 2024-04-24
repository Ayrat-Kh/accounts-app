#pragma once

#include <optional>

#include "auth.models.hpp"

namespace accounts
{
    std::optional<GoogleLoginValidationResult> validateGoogleLoginBody(const GoogleLoginRequest &body);
}