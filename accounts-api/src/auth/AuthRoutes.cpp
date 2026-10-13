#include "auth/AuthRoutes.hpp"

#include "http/JsonFields.hpp"

namespace accounts::auth {

using boost::asio::awaitable;
using http::Method;
using http::Request;
using http::Response;
using http::Status;

namespace {

// These endpoints are how a caller *gets* a token, so they can't require one.
constexpr http::RouteOptions publicRoute{.requiresAuth = false, .maxBodyBytes = {}};

boost::json::value toJson(const TokenPair &tokens) {
    return {
        {"access_token", tokens.accessToken},
        {"token_type", "Bearer"},
        {"expires_in", tokens.accessTokenExpiresIn.count()},
        {"refresh_token", tokens.refreshToken},
    };
}

// Each endpoint takes a JSON body with exactly one required string field.
Result<std::string> readStringField(const Request &request, std::string_view key) {
    auto body = http::parseJsonObject(request.body);
    if (!body) {
        return std::unexpected(body.error());
    }
    http::JsonFields fields(*body);
    std::string value = fields.requiredString(key);
    if (const auto &error = fields.firstError()) {
        return std::unexpected(*error);
    }
    return value;
}

awaitable<Response> signInWithGoogle(AuthService &authService, Request &request) {
    auto idToken = readStringField(request, "id_token");
    if (!idToken) {
        co_return http::errorResponse(idToken.error());
    }
    co_return http::respond(co_await authService.signInWithGoogle(*idToken), Status::Ok, toJson);
}

awaitable<Response> refreshTokens(AuthService &authService, Request &request) {
    auto refreshToken = readStringField(request, "refresh_token");
    if (!refreshToken) {
        co_return http::errorResponse(refreshToken.error());
    }
    co_return http::respond(co_await authService.refresh(*refreshToken), Status::Ok, toJson);
}

awaitable<Response> signOut(AuthService &authService, Request &request) {
    auto refreshToken = readStringField(request, "refresh_token");
    if (!refreshToken) {
        co_return http::errorResponse(refreshToken.error());
    }
    auto result = co_await authService.signOut(*refreshToken);
    co_return result ? http::noContentResponse() : http::errorResponse(result.error());
}

} // namespace

void registerAuthRoutes(http::Router &router, AuthService &authService) {
    auto bind = [&authService](auto handler) {
        return [&authService, handler](Request &request) { return handler(authService, request); };
    };
    router.add(Method::Post, "/auth/google", bind(signInWithGoogle), publicRoute);
    router.add(Method::Post, "/auth/token", bind(refreshTokens), publicRoute);
    router.add(Method::Post, "/auth/signout", bind(signOut), publicRoute);
}

} // namespace accounts::auth
