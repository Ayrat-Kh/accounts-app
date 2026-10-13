#pragma once

// POST /auth/google   {"id_token": "..."}       -> token pair
// POST /auth/token    {"refresh_token": "..."}  -> new token pair (old one revoked)
// POST /auth/signout  {"refresh_token": "..."}  -> 204
//
// Token pair: {"access_token", "token_type": "Bearer", "expires_in", "refresh_token"}

#include "auth/AuthService.hpp"
#include "http/Router.hpp"

namespace accounts::auth {

void registerAuthRoutes(http::Router &router, AuthService &authService);

} // namespace accounts::auth
