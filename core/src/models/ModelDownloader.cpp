#include "models/ModelDownloader.hpp"

#include "util/Files.hpp"

#include <algorithm>
#include <numeric>

namespace receipts::models {

namespace fs = std::filesystem;

namespace {

constexpr std::uint64_t pieceSize = 8 * 1024 * 1024; // the server's maximum per request

std::uint64_t sizeOrZero(const fs::path &path) {
    std::error_code error;
    const auto size = fs::file_size(path, error);
    return error ? 0 : size;
}

// Rejects names that could escape the models directory ("../x").
bool isSafeFileName(const std::string &name) {
    return !name.empty() && name.front() != '.' && name.find('/') == std::string::npos &&
           name.find('\\') == std::string::npos;
}

} // namespace

ModelDownloader::ModelDownloader(api::ApiClient &api, fs::path modelsDirectory, Logger &logger)
    : api_(api), modelsDirectory_(std::move(modelsDirectory)), logger_(logger) {}

fs::path ModelDownloader::localPath(const std::string &assetName) const {
    if (assetName.ends_with(".traineddata")) {
        return modelsDirectory_ / "tessdata" / assetName;
    }
    return modelsDirectory_ / assetName;
}

bool ModelDownloader::hasAll(const std::vector<std::string> &assetNames) const {
    return std::ranges::all_of(assetNames, [&](const std::string &name) { return fs::exists(localPath(name)); });
}

Result<void> ModelDownloader::downloadAll(ProgressListener *progress) {
    auto assets = api_.listAssets();
    if (!assets) {
        return std::unexpected(assets.error());
    }
    const std::uint64_t bytesTotal = std::accumulate(assets->begin(), assets->end(), std::uint64_t{0},
                                                     [](std::uint64_t sum, const auto &a) { return sum + a.size; });
    std::uint64_t bytesDone = 0;

    for (const api::AssetInfo &asset : *assets) {
        if (!isSafeFileName(asset.name)) {
            return fail(ErrorCode::Server, "refusing suspicious asset name: " + asset.name);
        }
        if (auto downloaded = download(asset, bytesDone, bytesTotal, progress); !downloaded) {
            return downloaded;
        }
    }
    return {};
}

Result<void> ModelDownloader::download(const api::AssetInfo &asset, std::uint64_t &bytesDoneOverall,
                                       std::uint64_t bytesTotal, ProgressListener *progress) {
    const fs::path target = localPath(asset.name);
    if (fs::exists(target)) {
        bytesDoneOverall += asset.size; // already complete and verified
        return {};
    }
    fs::create_directories(target.parent_path());
    const fs::path partial = target.string() + ".part";

    std::uint64_t offset = sizeOrZero(partial); // resume
    bytesDoneOverall += offset;
    while (offset < asset.size) {
        const std::uint64_t last = std::min(offset + pieceSize, asset.size) - 1;
        auto piece = api_.downloadAssetPiece(asset.name, offset, last);
        if (!piece) {
            return std::unexpected(piece.error());
        }
        if (piece->bytes.empty()) {
            return fail(ErrorCode::Server, "empty piece while downloading " + asset.name);
        }
        if (auto written = files::append(partial, piece->bytes); !written) {
            return written;
        }
        offset += piece->bytes.size();
        bytesDoneOverall += piece->bytes.size();
        if (progress != nullptr) {
            progress->onProgress(bytesDoneOverall, bytesTotal);
        }
    }

    auto digest = files::sha256OfFile(partial);
    if (!digest) {
        return std::unexpected(digest.error());
    }
    if (*digest != asset.sha256) {
        fs::remove(partial);
        return fail(ErrorCode::Network, asset.name + " arrived corrupted; it will be downloaded again");
    }
    fs::rename(partial, target);
    logger_.log(LogLevel::Info, "downloaded model file " + asset.name);
    return {};
}

} // namespace receipts::models
