#pragma once

// What a route handler receives. The Server copies everything out of
// uWebSockets' request object up front: uWS::HttpRequest is only valid
// during the synchronous callback, but our handlers are coroutines that run
// later. (Keeping that pointer around was one of the old code's crashes.)

#include "auth/AuthUser.hpp"

#include <map>
#include <optional>
#include <string>
#include <string_view>

namespace accounts::http {

enum class Method { Get, Head, Post, Put, Patch, Delete, Other };

using StringMap = std::map<std::string, std::string, std::less<>>;

struct Request {
    Method method = Method::Other;
    std::string path;      // without the query string
    StringMap queryParams; // percent-decoded
    StringMap headers;     // names in lower case, as uWebSockets delivers them
    std::string body;
    StringMap pathParams;         // "/v1/receipts/:id" -> {"id", "..."}, filled by the Router
    std::optional<AuthUser> user; // filled by the Router for routes that require auth

    std::string_view header(std::string_view lowerCaseName) const {
        auto found = headers.find(lowerCaseName);
        return found == headers.end() ? std::string_view{} : std::string_view(found->second);
    }

    std::string_view pathParam(std::string_view name) const {
        auto found = pathParams.find(name);
        return found == pathParams.end() ? std::string_view{} : std::string_view(found->second);
    }

    std::optional<std::string> queryParam(std::string_view name) const {
        auto found = queryParams.find(name);
        return found == queryParams.end() ? std::nullopt : std::optional(found->second);
    }

    // Only valid on routes that require auth (the default): the router has
    // already rejected unauthenticated requests before the handler runs.
    const AuthUser &authenticatedUser() const { return user.value(); }
};

} // namespace accounts::http
