#pragma once

// ObjectStore backed by Garage through the AWS SDK.
//
// The SDK only offers blocking calls, so each one runs on the
// BlockingExecutor's thread pool while the event loop keeps serving requests.
// The SDK must be initialised once per process; see AwsSdkScope.

#include "config/Config.hpp"
#include "storage/ObjectStore.hpp"
#include "util/BlockingExecutor.hpp"

#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>

namespace accounts::storage {

// RAII for Aws::InitAPI / Aws::ShutdownAPI. Create one in main() before any S3ObjectStore.
class AwsSdkScope {
public:
    AwsSdkScope() { Aws::InitAPI(options_); }
    ~AwsSdkScope() { Aws::ShutdownAPI(options_); }
    AwsSdkScope(const AwsSdkScope &) = delete;
    AwsSdkScope &operator=(const AwsSdkScope &) = delete;

private:
    Aws::SDKOptions options_;
};

class S3ObjectStore : public ObjectStore {
public:
    S3ObjectStore(const S3Config &config, BlockingExecutor &blocking);

    boost::asio::awaitable<Result<void>> put(std::string key, std::string bytes, std::string contentType) override;
    boost::asio::awaitable<Result<std::optional<StoredObject>>> get(std::string key,
                                                                    std::optional<ByteRange> range) override;
    boost::asio::awaitable<Result<void>> remove(std::string key) override;

private:
    std::string bucket_;
    BlockingExecutor &blocking_;
    Aws::S3::S3Client client_;
};

} // namespace accounts::storage
