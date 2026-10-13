#include "health/HealthRoutes.hpp"

namespace accounts::health {

using boost::asio::awaitable;
using http::Method;
using http::Request;
using http::Response;
using http::Status;

namespace {

constexpr http::RouteOptions publicRoute{.requiresAuth = false, .maxBodyBytes = {}};

} // namespace

void registerHealthRoutes(http::Router &router, db::PgPool &pool) {
    router.add(
        Method::Get, "/healthz",
        [](Request &) -> awaitable<Response> { co_return http::jsonResponse(Status::Ok, {{"status", "ok"}}); },
        publicRoute);

    router.add(
        Method::Get, "/readyz",
        [&pool](Request &) -> awaitable<Response> {
            auto result = co_await pool.query("SELECT 1");
            if (!result) {
                co_return http::errorResponse(result.error());
            }
            co_return http::jsonResponse(Status::Ok, {{"status", "ready"}});
        },
        publicRoute);

    router.add(
        Method::Get, "/v1/version",
        [](Request &) -> awaitable<Response> { co_return http::jsonResponse(Status::Ok, {{"api", "1"}}); },
        publicRoute);
}

} // namespace accounts::health
