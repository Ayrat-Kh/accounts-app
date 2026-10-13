#include "storage/InMemoryObjectStore.hpp"

#include <algorithm>

namespace accounts::storage {

using boost::asio::awaitable;

awaitable<Result<void>> InMemoryObjectStore::put(std::string key, std::string bytes, std::string contentType) {
    objects_[std::move(key)] = {std::move(bytes), std::move(contentType)};
    co_return Result<void>{};
}

awaitable<Result<std::optional<StoredObject>>> InMemoryObjectStore::get(std::string key,
                                                                        std::optional<ByteRange> range) {
    const auto found = objects_.find(key);
    if (found == objects_.end()) {
        co_return std::nullopt;
    }
    const std::string &bytes = found->second.bytes;
    std::string slice = bytes;
    if (range) {
        const auto first = std::min<std::uint64_t>(range->first, bytes.size());
        const auto last = std::min<std::uint64_t>(range->last, bytes.empty() ? 0 : bytes.size() - 1);
        slice = first <= last ? bytes.substr(first, last - first + 1) : std::string{};
    }
    co_return StoredObject{std::move(slice), found->second.contentType, bytes.size()};
}

awaitable<Result<void>> InMemoryObjectStore::remove(std::string key) {
    objects_.erase(key);
    co_return Result<void>{};
}

} // namespace accounts::storage
