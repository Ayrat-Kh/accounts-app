// ReceiptsService: the composition root of the core library. It owns every
// component, wires them together according to CoreConfig, and keeps the
// public methods thin, each delegating to the component that does the work.

#include "receipts/ReceiptsService.hpp"

#include "api/ApiClient.hpp"
#include "app/SyncEngine.hpp"
#include "extract/EmbeddingLineScorer.hpp"
#include "extract/KeywordLineScorer.hpp"
#include "extract/ReceiptScanner.hpp"
#include "inference/LlamaEmbeddingEngine.hpp"
#include "inference/OpenAiCompatibleEngine.hpp"
#include "models/ModelDownloader.hpp"
#include "net/CurlHttpTransport.hpp"
#include "ocr/TesseractOcrEngine.hpp"
#include "store/LocalReceiptStore.hpp"
#include "util/Files.hpp"
#include "util/Time.hpp"

#include <filesystem>
#include <mutex>
#include <random>

namespace receipts {

namespace fs = std::filesystem;

namespace {

class SilentLogger : public Logger {
public:
    void log(LogLevel, std::string_view) override {}
};

// Model files the scanner needs: one traineddata per OCR language, plus the
// embedding model if it runs on the device.
std::vector<std::string> requiredModelFiles(const CoreConfig &config) {
    std::vector<std::string> files;
    std::string_view languages = config.ocrLanguages;
    while (!languages.empty()) {
        const auto plus = languages.find('+');
        files.push_back(std::string(languages.substr(0, plus)) + ".traineddata");
        languages = plus == std::string_view::npos ? std::string_view{} : languages.substr(plus + 1);
    }
    if (config.embedding && config.embedding->kind == EngineConfig::Kind::Local) {
        files.push_back(config.embedding->modelFile);
    }
    return files;
}

std::string newLocalId() {
    static std::mutex mutex;
    static std::mt19937_64 random{std::random_device{}()};
    std::lock_guard lock(mutex);
    char buffer[24];
    std::snprintf(buffer, sizeof buffer, "local-%016llx", static_cast<unsigned long long>(random()));
    return buffer;
}

} // namespace

// Everything a scan needs. Built on the first scan, because the model files
// may only arrive (downloadModels) after the service was created.
struct ScanPipeline {
    std::unique_ptr<ocr::TesseractOcrEngine> ocr;
    std::unique_ptr<inference::EmbeddingEngine> embedding;              // null: keywords only
    std::shared_ptr<inference::OpenAiCompatibleEngine> completionEngine;  // null: no LLM fallback
    std::unique_ptr<extract::LineScorer> scorer;
    extract::KeywordLineScorer keywordScorer;
    std::unique_ptr<extract::LlmTotalExtractor> llm;
    std::unique_ptr<extract::PriceExtractor> extractor;
    std::unique_ptr<extract::ReceiptScanner> scanner;
};

struct ReceiptsService::Impl {
    CoreConfig config;
    std::shared_ptr<SecureStore> secureStore;
    std::shared_ptr<Logger> logger;
    fs::path imagesDirectory;
    fs::path modelsDirectory;

    std::shared_ptr<net::HttpTransport> transport;
    api::ApiClient api;
    std::unique_ptr<store::LocalReceiptStore> store;
    app::SyncEngine sync;
    models::ModelDownloader models;

    std::mutex scanMutex;
    std::unique_ptr<ScanPipeline> scanPipeline;

    Impl(CoreConfig configuration, std::shared_ptr<SecureStore> secrets, std::shared_ptr<Logger> log,
         std::unique_ptr<store::LocalReceiptStore> localStore)
        : config(std::move(configuration)), secureStore(std::move(secrets)), logger(std::move(log)),
          imagesDirectory(fs::path(config.dataDirectory) / "images"),
          modelsDirectory(fs::path(config.dataDirectory) / "models"),
          transport(std::make_shared<net::CurlHttpTransport>(config.caCertificatesDirectory)),
          api(transport, config.apiBaseUrl, secureStore), store(std::move(localStore)), sync(api, *store, *logger),
          models(api, modelsDirectory, *logger) {}

    Result<std::unique_ptr<inference::EmbeddingEngine>> makeEmbeddingEngine(const EngineConfig &engine) {
        if (engine.kind == EngineConfig::Kind::Local) {
            auto local = inference::LlamaEmbeddingEngine::load((modelsDirectory / engine.modelFile).string());
            if (!local) {
                return std::unexpected(local.error());
            }
            return std::unique_ptr<inference::EmbeddingEngine>(std::move(*local));
        }
        return std::unique_ptr<inference::EmbeddingEngine>(
            std::make_unique<inference::OpenAiCompatibleEngine>(transport, engine, apiKeyFor(engine)));
    }

    std::optional<std::string> apiKeyFor(const EngineConfig &engine) {
        return engine.apiKeySecureStoreKey ? secureStore->read(*engine.apiKeySecureStoreKey) : std::nullopt;
    }

    Result<extract::ReceiptScanner *> scanner() {
        std::lock_guard lock(scanMutex);
        if (scanPipeline) {
            return scanPipeline->scanner.get();
        }
        auto pipeline = std::make_unique<ScanPipeline>();

        auto ocr = ocr::TesseractOcrEngine::create((modelsDirectory / "tessdata").string(), config.ocrLanguages);
        if (!ocr) {
            return std::unexpected(ocr.error());
        }
        pipeline->ocr = std::move(*ocr);

        if (config.embedding) {
            if (auto embedding = makeEmbeddingEngine(*config.embedding)) {
                pipeline->embedding = std::move(*embedding);
                pipeline->scorer = std::make_unique<extract::EmbeddingLineScorer>(*pipeline->embedding,
                                                                                  config.embedding->textPrefix);
            } else {
                logger->log(LogLevel::Warning, "embedding model unavailable, using keywords: " +
                                                   embedding.error().message);
            }
        }
        if (!pipeline->scorer) {
            pipeline->scorer = std::make_unique<extract::KeywordLineScorer>();
        }

        if (config.completion) {
            pipeline->completionEngine = std::make_shared<inference::OpenAiCompatibleEngine>(
                transport, *config.completion, apiKeyFor(*config.completion));
            pipeline->llm = std::make_unique<extract::LlmTotalExtractor>(*pipeline->completionEngine);
        }

        pipeline->extractor = std::make_unique<extract::PriceExtractor>(
            *pipeline->scorer, pipeline->keywordScorer, pipeline->llm.get(), *logger,
            extract::PriceExtractorOptions{.askLlmBelowConfidence = config.askCompletionBelowConfidence,
                                           .defaultCurrency = config.defaultCurrency});
        pipeline->scanner = std::make_unique<extract::ReceiptScanner>(*pipeline->ocr, *pipeline->extractor);
        scanPipeline = std::move(pipeline);
        return scanPipeline->scanner.get();
    }

    // Copies the photo into our own directory: the host's file (a camera
    // temp file) may be deleted as soon as the receipt is saved.
    Result<std::optional<std::string>> keepImage(const std::optional<std::string> &source, const std::string &localId) {
        if (!source) {
            return std::nullopt;
        }
        std::error_code error;
        fs::create_directories(imagesDirectory, error);
        const fs::path target = imagesDirectory / (localId + fs::path(*source).extension().string());
        fs::copy_file(*source, target, fs::copy_options::overwrite_existing, error);
        if (error) {
            return fail(ErrorCode::InvalidInput, "cannot copy the photo: " + error.message());
        }
        return target.string();
    }
};

Result<std::unique_ptr<ReceiptsService>> ReceiptsService::create(CoreConfig config,
                                                                 std::shared_ptr<SecureStore> secureStore,
                                                                 std::shared_ptr<Logger> logger) {
    if (config.dataDirectory.empty() || !secureStore) {
        return fail(ErrorCode::InvalidInput, "a data directory and a SecureStore are required");
    }
    std::error_code error;
    fs::create_directories(config.dataDirectory, error);
    if (error) {
        return fail(ErrorCode::InvalidInput, "cannot create " + config.dataDirectory + ": " + error.message());
    }
    auto store = store::LocalReceiptStore::open((fs::path(config.dataDirectory) / "receipts.db").string());
    if (!store) {
        return std::unexpected(store.error());
    }
    if (!logger) {
        logger = std::make_shared<SilentLogger>();
    }
    auto impl = std::make_unique<Impl>(std::move(config), std::move(secureStore), std::move(logger), std::move(*store));
    return std::unique_ptr<ReceiptsService>(new ReceiptsService(std::move(impl)));
}

ReceiptsService::ReceiptsService(std::unique_ptr<Impl> impl) : impl_(std::move(impl)) {}
ReceiptsService::~ReceiptsService() = default;

Result<void> ReceiptsService::signInWithGoogle(const std::string &idToken) {
    return impl_->api.signInWithGoogle(idToken);
}

void ReceiptsService::signOut() {
    impl_->api.signOut();
}

bool ReceiptsService::isSignedIn() const {
    return impl_->api.isSignedIn();
}

bool ReceiptsService::modelsReady() const {
    return impl_->models.hasAll(requiredModelFiles(impl_->config));
}

Result<void> ReceiptsService::downloadModels(ProgressListener *progress) {
    return impl_->models.downloadAll(progress);
}

Result<ScanResult> ReceiptsService::scanReceipt(const std::string &imagePath) {
    auto scanner = impl_->scanner();
    if (!scanner) {
        return std::unexpected(scanner.error());
    }
    return (*scanner)->scan(imagePath);
}

Result<Receipt> ReceiptsService::saveReceipt(const ReceiptDraft &draft) {
    if (draft.money.amountMinor < 0 || draft.money.currency.size() != 3) {
        return fail(ErrorCode::InvalidInput, "an amount and a three-letter currency are required");
    }
    const std::string localId = newLocalId();
    auto imagePath = impl_->keepImage(draft.imagePath, localId);
    if (!imagePath) {
        return std::unexpected(imagePath.error());
    }
    return impl_->store->addPending(draft, localId, *imagePath);
}

Result<std::vector<Receipt>> ReceiptsService::listReceipts() {
    return impl_->store->listVisible();
}

Result<void> ReceiptsService::deleteReceipt(const std::string &localId) {
    auto receipt = impl_->store->find(localId);
    if (!receipt) {
        return std::unexpected(receipt.error());
    }
    if (!*receipt) {
        return fail(ErrorCode::NotFound, "no such receipt");
    }
    if (auto marked = impl_->store->markPendingDelete(localId); !marked) {
        return marked;
    }
    if ((*receipt)->imagePath) {
        std::error_code ignored;
        fs::remove(*(*receipt)->imagePath, ignored);
    }
    return {};
}

Result<SyncReport> ReceiptsService::sync() {
    return impl_->sync.sync();
}

} // namespace receipts
