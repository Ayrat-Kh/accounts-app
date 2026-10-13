#pragma once

// Downloads the scanner's model files (EmbeddingGemma GGUF, Tesseract
// traineddata) from accounts-api's /v1/assets into the models directory.
//
// Each file arrives in 8 MiB pieces appended to "<name>.part", so an
// interrupted download resumes where it stopped. A file only gets its real
// name after its SHA-256 matches the manifest; a corrupt download is deleted
// and started over.

#include "api/ApiClient.hpp"
#include "receipts/Ports.hpp"

#include <filesystem>
#include <vector>

namespace receipts::models {

class ModelDownloader {
public:
    ModelDownloader(api::ApiClient &api, std::filesystem::path modelsDirectory, Logger &logger);

    // Where an asset lives locally: traineddata files go to tessdata/, as Tesseract expects.
    std::filesystem::path localPath(const std::string &assetName) const;

    bool hasAll(const std::vector<std::string> &assetNames) const;

    Result<void> downloadAll(ProgressListener *progress);

private:
    Result<void> download(const api::AssetInfo &asset, std::uint64_t &bytesDoneOverall, std::uint64_t bytesTotal,
                          ProgressListener *progress);

    api::ApiClient &api_;
    std::filesystem::path modelsDirectory_;
    Logger &logger_;
};

} // namespace receipts::models
