#pragma once

#include "storage/ObjectStore.hpp"

#include <map>

namespace accounts::storage {

// A map pretending to be S3. Used by tests and handy for local experiments.
class InMemoryObjectStore : public ObjectStore {
public:
    boost::asio::awaitable<Result<void>> put(std::string key, std::string bytes, std::string contentType) override;
    boost::asio::awaitable<Result<std::optional<StoredObject>>> get(std::string key,
                                                                    std::optional<ByteRange> range) override;
    boost::asio::awaitable<Result<void>> remove(std::string key) override;

    std::size_t size() const { return objects_.size(); }

private:
    struct Entry {
        std::string bytes;
        std::string contentType;
    };
    std::map<std::string, Entry> objects_;
};

} // namespace accounts::storage
