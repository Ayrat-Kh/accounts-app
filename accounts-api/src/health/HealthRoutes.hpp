#pragma once

// GET /healthz      liveness: the process is serving requests
// GET /readyz       readiness: Postgres answers too (Kubernetes routes traffic only when ready)
// GET /v1/version   {"api": "1"}

#include "db/PgPool.hpp"
#include "http/Router.hpp"

namespace accounts::health {

void registerHealthRoutes(http::Router &router, db::PgPool &pool);

} // namespace accounts::health
