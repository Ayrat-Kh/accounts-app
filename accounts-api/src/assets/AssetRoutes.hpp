#pragma once

// Model files for the on-device scanner (EmbeddingGemma GGUF, Tesseract
// traineddata), served from the object store under "assets/".
//
// GET /v1/assets          -> assets/manifest.json: [{"name", "size", "sha256"}, ...]
// GET /v1/assets/:name    -> one file, downloaded in pieces with
//                            "Range: bytes=first-last" (at most 8 MiB per
//                            request, answered with 206 + Content-Range)
//
// Why ranges: a model is hundreds of MB. Pieces keep server memory bounded,
// and a phone that loses its connection resumes where it stopped instead of
// starting over.

#include "http/Router.hpp"
#include "storage/ObjectStore.hpp"

namespace accounts::assets {

void registerAssetRoutes(http::Router &router, storage::ObjectStore &objectStore);

// "bytes=0-1023" -> {0, 1023}. Exposed for tests.
std::optional<storage::ByteRange> parseRangeHeader(std::string_view header);

} // namespace accounts::assets
