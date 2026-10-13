#pragma once

// Where receipt photos and model assets live. In production this is Garage
// (S3-compatible, S3ObjectStore); tests use InMemoryObjectStore.

#include "util/Error.hpp"

#include <boost/asio/awaitable.hpp>

#include <cstdint>
#include <optional>
#include <string>

namespace accounts::storage {

struct StoredObject {
    std::string bytes;
    std::string contentType;
    std::uint64_t totalSize; // of the whole object, even when only a range was read
};

// Inclusive byte range, as in "Range: bytes=first-last".
struct ByteRange {
    std::uint64_t first;
    std::uint64_t last;
};

class ObjectStore {
public:
    virtual ~ObjectStore() = default;

    virtual boost::asio::awaitable<Result<void>> put(std::string key, std::string bytes, std::string contentType) = 0;

    // nullopt when the object does not exist.
    virtual boost::asio::awaitable<Result<std::optional<StoredObject>>>
    get(std::string key, std::optional<ByteRange> range = std::nullopt) = 0;

    virtual boost::asio::awaitable<Result<void>> remove(std::string key) = 0;
};

} // namespace accounts::storage
