// receipts-cli: run the receipt scanner from a terminal (macOS or Linux).
//
//   receipts-cli scan photo.jpg                              # EmbeddingGemma in-process
//   receipts-cli scan photo.jpg --engine keywords            # no model, keyword matching
//   receipts-cli scan photo.jpg --engine remote \
//       --base-url http://localhost:8080 --model embeddinggemma-2   # llama-server / oMLX / Ollama
//   receipts-cli scan photo.jpg --completion-url http://localhost:8000 \
//       --completion-model gemma-4-12b                       # add an LLM second opinion
//
// Prints the ScanResult as JSON. Models are read from --models (default:
// core/testdata/models, relative to the repository root; fill it with
// scripts/download-models.sh).

#include "extract/EmbeddingLineScorer.hpp"
#include "extract/KeywordLineScorer.hpp"
#include "extract/ReceiptScanner.hpp"
#include "inference/LlamaEmbeddingEngine.hpp"
#include "inference/OpenAiCompatibleEngine.hpp"
#include "net/CurlHttpTransport.hpp"
#include "ocr/TesseractOcrEngine.hpp"

#include <nlohmann/json.hpp>

#include <filesystem>
#include <iostream>
#include <map>

using namespace receipts;
using nlohmann::json;

namespace {

struct Options {
    std::string imagePath;
    std::string modelsDirectory = "core/testdata/models";
    std::string languages = "eng+deu+rus";
    std::string engine = "local";  // local | remote | keywords
    std::string baseUrl;
    std::string model;
    std::string completionUrl;
    std::string completionModel;
    std::string prefix = EngineConfig{}.textPrefix;  // the embedding model's task prefix
};

class StderrLogger : public Logger {
public:
    void log(LogLevel, std::string_view message) override { std::cerr << message << '\n'; }
};

int usage() {
    std::cerr << "usage: receipts-cli scan <image> [--models DIR] [--languages eng+deu+rus]\n"
                 "         [--engine local|remote|keywords] [--base-url URL] [--model NAME]\n"
                 "         [--completion-url URL] [--completion-model NAME] [--prefix TEXT]\n";
    return 2;
}

std::optional<Options> parseArguments(int argc, char **argv) {
    if (argc < 3 || std::string_view(argv[1]) != "scan") {
        return std::nullopt;
    }
    Options options;
    options.imagePath = argv[2];
    const std::map<std::string_view, std::string *> flags = {
        {"--models", &options.modelsDirectory}, {"--languages", &options.languages},
        {"--engine", &options.engine},          {"--base-url", &options.baseUrl},
        {"--model", &options.model},            {"--completion-url", &options.completionUrl},
        {"--completion-model", &options.completionModel}, {"--prefix", &options.prefix},
    };
    for (int i = 3; i < argc; i += 2) {
        const auto flag = flags.find(argv[i]);
        if (flag == flags.end() || i + 1 >= argc) {
            return std::nullopt;
        }
        *flag->second = argv[i + 1];
    }
    return options;
}

json toJson(const ScanResult &result) {
    auto money = [](const Money &m) { return json{{"amount_minor", m.amountMinor}, {"currency", m.currency}}; };
    json candidates = json::array();
    for (const auto &candidate : result.candidates) {
        candidates.push_back({{"money", money(candidate.money)}, {"line", candidate.lineText}, {"score", candidate.score}});
    }
    static const std::map<ExtractionSource, const char *> sources = {
        {ExtractionSource::Embedding, "embedding"}, {ExtractionSource::Keywords, "keywords"}, {ExtractionSource::Llm, "llm"}};
    return {
        {"total", result.total ? money(*result.total) : json(nullptr)},
        {"purchased_at", result.purchasedAt ? json(*result.purchasedAt) : json(nullptr)},
        {"merchant", result.merchant ? json(*result.merchant) : json(nullptr)},
        {"confidence", result.confidence},
        {"source", sources.at(result.source)},
        {"candidates", candidates},
        {"ocr_lines", result.ocrLines},
    };
}

} // namespace

int main(int argc, char **argv) {
    const auto options = parseArguments(argc, argv);
    if (!options) {
        return usage();
    }
    StderrLogger logger;
    auto transport = std::make_shared<net::CurlHttpTransport>();

    auto ocr = ocr::TesseractOcrEngine::create(
        (std::filesystem::path(options->modelsDirectory) / "tessdata").string(), options->languages);
    if (!ocr) {
        std::cerr << ocr.error().message << '\n';
        return 1;
    }

    // Pick the line scorer.
    std::unique_ptr<inference::EmbeddingEngine> embeddingEngine;
    if (options->engine == "local") {
        auto local = inference::LlamaEmbeddingEngine::load(
            (std::filesystem::path(options->modelsDirectory) / EngineConfig{}.modelFile).string());
        if (!local) {
            std::cerr << local.error().message << '\n';
            return 1;
        }
        embeddingEngine = std::move(*local);
    } else if (options->engine == "remote") {
        embeddingEngine = std::make_unique<inference::OpenAiCompatibleEngine>(
            transport,
            EngineConfig{.kind = EngineConfig::Kind::OpenAiCompatible, .baseUrl = options->baseUrl, .model = options->model},
            std::nullopt);
    } else if (options->engine != "keywords") {
        return usage();
    }
    extract::KeywordLineScorer keywordScorer;
    std::unique_ptr<extract::EmbeddingLineScorer> embeddingScorer;
    if (embeddingEngine) {
        embeddingScorer = std::make_unique<extract::EmbeddingLineScorer>(*embeddingEngine, options->prefix);
    }
    extract::LineScorer &scorer = embeddingScorer ? static_cast<extract::LineScorer &>(*embeddingScorer) : keywordScorer;

    // Optional LLM second opinion.
    std::unique_ptr<inference::OpenAiCompatibleEngine> completionEngine;
    std::unique_ptr<extract::LlmTotalExtractor> llm;
    if (!options->completionUrl.empty()) {
        completionEngine = std::make_unique<inference::OpenAiCompatibleEngine>(
            transport,
            EngineConfig{.kind = EngineConfig::Kind::OpenAiCompatible, .baseUrl = options->completionUrl,
                         .model = options->completionModel, .timeout = std::chrono::seconds(120)},
            std::nullopt);
        llm = std::make_unique<extract::LlmTotalExtractor>(*completionEngine);
    }

    extract::PriceExtractor extractor(scorer, keywordScorer, llm.get(), logger);
    extract::ReceiptScanner scanner(**ocr, extractor);
    const auto result = scanner.scan(options->imagePath);
    if (!result) {
        std::cerr << result.error().message << '\n';
        return 1;
    }
    std::cout << toJson(*result).dump(2) << '\n';
    return 0;
}
