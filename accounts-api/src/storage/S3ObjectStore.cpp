#include "storage/S3ObjectStore.hpp"

#include <aws/core/auth/AWSCredentials.h>
#include <aws/s3/model/DeleteObjectRequest.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/s3/model/PutObjectRequest.h>

#include <charconv>
#include <format>
#include <sstream>

namespace accounts::storage {

using boost::asio::awaitable;

namespace {

// Settings for Garage rather than AWS (same as obsync's s3_store.cpp):
// path-style URLs (no wildcard DNS for <bucket>.<host>), checksums only when
// required (Garage rejects some of the SDK's newer defaults), short connect timeout.
Aws::S3::S3ClientConfiguration clientConfiguration(const S3Config &config) {
    Aws::S3::S3ClientConfiguration configuration;
    configuration.endpointOverride = config.endpoint;
    configuration.region = config.region;
    configuration.scheme =
        config.endpoint.starts_with("https://") ? Aws::Http::Scheme::HTTPS : Aws::Http::Scheme::HTTP;
    configuration.connectTimeoutMs = 3000;
    configuration.requestTimeoutMs = 30000;
    configuration.checksumConfig.requestChecksumCalculation = Aws::Client::RequestChecksumCalculation::WHEN_REQUIRED;
    configuration.useVirtualAddressing = false;
    return configuration;
}

AppError s3Error(std::string_view operation, const std::string &key, const Aws::S3::S3Error &error) {
    return {ErrorCode::UpstreamFailure, std::format("S3 {} {}: {}", operation, key, error.GetMessage().c_str())};
}

bool isNotFound(const Aws::S3::S3Error &error) {
    return error.GetResponseCode() == Aws::Http::HttpResponseCode::NOT_FOUND ||
           error.GetErrorType() == Aws::S3::S3Errors::NO_SUCH_KEY ||
           error.GetErrorType() == Aws::S3::S3Errors::RESOURCE_NOT_FOUND;
}

// "bytes 0-99/1234" -> 1234
std::optional<std::uint64_t> totalSizeFromContentRange(std::string_view contentRange) {
    const auto slash = contentRange.rfind('/');
    if (slash == std::string_view::npos) {
        return std::nullopt;
    }
    std::uint64_t total = 0;
    const auto digits = contentRange.substr(slash + 1);
    if (std::from_chars(digits.data(), digits.data() + digits.size(), total).ec != std::errc{}) {
        return std::nullopt;
    }
    return total;
}

} // namespace

S3ObjectStore::S3ObjectStore(const S3Config &config, BlockingExecutor &blocking)
    : bucket_(config.bucket), blocking_(blocking),
      client_(Aws::Auth::AWSCredentials(config.accessKey, config.secretKey), nullptr, clientConfiguration(config)) {}

awaitable<Result<void>> S3ObjectStore::put(std::string key, std::string bytes, std::string contentType) {
    co_return co_await blocking_.run([&]() -> Result<void> {
        Aws::S3::Model::PutObjectRequest request;
        request.SetBucket(bucket_);
        request.SetKey(key);
        request.SetContentType(contentType);
        auto body = Aws::MakeShared<Aws::StringStream>("S3ObjectStore");
        body->write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
        request.SetBody(body);
        request.SetContentLength(static_cast<long long>(bytes.size()));

        auto outcome = client_.PutObject(request);
        if (!outcome.IsSuccess()) {
            return std::unexpected(s3Error("put", key, outcome.GetError()));
        }
        return {};
    });
}

awaitable<Result<std::optional<StoredObject>>> S3ObjectStore::get(std::string key, std::optional<ByteRange> range) {
    co_return co_await blocking_.run([&]() -> Result<std::optional<StoredObject>> {
        Aws::S3::Model::GetObjectRequest request;
        request.SetBucket(bucket_);
        request.SetKey(key);
        if (range) {
            request.SetRange(std::format("bytes={}-{}", range->first, range->last));
        }

        auto outcome = client_.GetObject(request);
        if (!outcome.IsSuccess()) {
            if (isNotFound(outcome.GetError())) {
                return std::nullopt;
            }
            return std::unexpected(s3Error("get", key, outcome.GetError()));
        }

        auto &result = outcome.GetResult();
        std::ostringstream bytes;
        bytes << result.GetBody().rdbuf();
        const auto contentLength = static_cast<std::uint64_t>(result.GetContentLength());
        return StoredObject{
            .bytes = std::move(bytes).str(),
            .contentType = result.GetContentType(),
            .totalSize = totalSizeFromContentRange(result.GetContentRange()).value_or(contentLength),
        };
    });
}

awaitable<Result<void>> S3ObjectStore::remove(std::string key) {
    co_return co_await blocking_.run([&]() -> Result<void> {
        Aws::S3::Model::DeleteObjectRequest request;
        request.SetBucket(bucket_);
        request.SetKey(key);
        auto outcome = client_.DeleteObject(request);
        if (!outcome.IsSuccess() && !isNotFound(outcome.GetError())) {
            return std::unexpected(s3Error("delete", key, outcome.GetError()));
        }
        return {};
    });
}

} // namespace accounts::storage
