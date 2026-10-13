#pragma once

// POST   /v1/receipts                 create             -> 201 receipt
// GET    /v1/receipts?cursor=&limit=  list, newest first -> {"items": [...], "next_cursor": "..."|null}
// GET    /v1/receipts/:id                                -> receipt
// PATCH  /v1/receipts/:id             partial update     -> receipt
// DELETE /v1/receipts/:id                                -> 204
// PUT    /v1/receipts/:id/image       raw JPEG/PNG/WebP body + X-Content-SHA256 header -> receipt
// GET    /v1/receipts/:id/image                          -> the image bytes
//
// All of them require an access token and only ever see the caller's receipts.

#include "http/Router.hpp"
#include "receipts/ReceiptService.hpp"

namespace accounts::receipts {

void registerReceiptRoutes(http::Router &router, ReceiptService &service, std::size_t maxImageBytes);

} // namespace accounts::receipts
