// End-to-end accuracy on the synthetic receipts in testdata/receipts: real
// Tesseract OCR, then price extraction with each line scorer.
//
// Needs the model files: run scripts/download-models.sh core/testdata/models
// (or point RECEIPTS_TEST_MODELS_DIR at a copy). Skipped without them.

#include "extract/EmbeddingLineScorer.hpp"
#include "extract/KeywordLineScorer.hpp"
#include "extract/PriceExtractor.hpp"
#include "inference/LlamaEmbeddingEngine.hpp"
#include "ocr/TesseractOcrEngine.hpp"
#include "receipts/Config.hpp"
#include "support/Fakes.hpp"
#include "util/Files.hpp"

#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>

#include <cstdlib>
#include <filesystem>
#include <format>
#include <iostream>

using namespace receipts;
namespace fs = std::filesystem;

namespace {

const fs::path receiptsDirectory = fs::path(RECEIPTS_TESTDATA_DIR) / "receipts";

fs::path modelsDirectory() {
    const char *fromEnvironment = std::getenv("RECEIPTS_TEST_MODELS_DIR");
    return fromEnvironment ? fs::path(fromEnvironment) : fs::path(RECEIPTS_TESTDATA_DIR) / "models";
}

struct Expectation {
    std::string file;
    Money total; // currency empty when the receipt doesn't print one
};

std::vector<Expectation> expectations() {
    const auto json = nlohmann::json::parse(files::readAll(receiptsDirectory / "expected.json").value());
    std::vector<Expectation> result;
    for (const auto &[file, total] : json.items()) {
        const auto &currency = total.at("currency");
        result.push_back({file,
                          {total.at("amount_minor").get<std::int64_t>(),
                           currency.is_null() ? std::string{} : currency.get<std::string>()}});
    }
    return result;
}

// OCR once per receipt; both scorers then work on the same text.
std::vector<std::pair<Expectation, ocr::OcrPage>> recognizeAll(ocr::OcrEngine &ocr) {
    std::vector<std::pair<Expectation, ocr::OcrPage>> pages;
    for (const auto &expectation : expectations()) {
        pages.emplace_back(expectation, ocr.recognize((receiptsDirectory / expectation.file).string()).value());
    }
    return pages;
}

double accuracy(extract::LineScorer &scorer, const std::vector<std::pair<Expectation, ocr::OcrPage>> &pages) {
    extract::KeywordLineScorer fallback;
    testing::RecordingLogger logger;
    extract::PriceExtractor extractor(scorer, fallback, nullptr, logger);

    int correct = 0;
    for (const auto &[expectation, page] : pages) {
        const auto result = extractor.extract(page).value();
        const bool isCorrect =
            result.total && result.total->amountMinor == expectation.total.amountMinor &&
            (expectation.total.currency.empty() || result.total->currency == expectation.total.currency);
        correct += isCorrect ? 1 : 0;
        std::cout << std::format(
            "  {:<26} expected {:>7} {}  got {:>7} {}  confidence {:.2f}  {}\n", expectation.file,
            expectation.total.amountMinor, expectation.total.currency, result.total ? result.total->amountMinor : -1,
            result.total ? result.total->currency : "---", result.confidence, isCorrect ? "ok" : "WRONG");
    }
    return static_cast<double>(correct) / static_cast<double>(pages.size());
}

} // namespace

TEST_CASE("receipt totals are extracted accurately from photos", "[accuracy]") {
    const fs::path models = modelsDirectory();
    if (!fs::exists(models / "tessdata" / "eng.traineddata")) {
        SKIP("model files missing: scripts/download-models.sh core/testdata/models");
    }
    auto ocr = ocr::TesseractOcrEngine::create((models / "tessdata").string(), "eng+deu+rus").value();
    const auto pages = recognizeAll(*ocr);

    std::cout << "Keyword scorer:\n";
    extract::KeywordLineScorer keywords;
    const double keywordAccuracy = accuracy(keywords, pages);
    std::cout << std::format("  -> {:.0f}% correct\n", keywordAccuracy * 100);

    const fs::path embeddingModel = models / "embeddinggemma-2-Q8_0.gguf";
    if (!fs::exists(embeddingModel)) {
        WARN("embedding model missing; only the keyword scorer was measured");
        CHECK(keywordAccuracy >= 0.6);
        return;
    }
    auto engine = inference::LlamaEmbeddingEngine::load(embeddingModel.string()).value();
    extract::EmbeddingLineScorer embedding(*engine, EngineConfig{}.textPrefix);
    std::cout << "EmbeddingGemma scorer:\n";
    const double embeddingAccuracy = accuracy(embedding, pages);
    std::cout << std::format("  -> {:.0f}% correct\n", embeddingAccuracy * 100);

    CHECK(embeddingAccuracy >= 0.9);
    CHECK(embeddingAccuracy >= keywordAccuracy);
}
