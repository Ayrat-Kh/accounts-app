#include "extract/PriceExtractor.hpp"
#include "extract/KeywordLineScorer.hpp"
#include "support/Fakes.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace receipts;
using namespace receipts::extract;

namespace {

// Builds an OcrPage from plain lines, spaced evenly top to bottom.
ocr::OcrPage page(const std::vector<std::string> &lines) {
    ocr::OcrPage result;
    result.imageHeight = static_cast<int>(lines.size()) * 40;
    for (std::size_t i = 0; i < lines.size(); ++i) {
        result.lines.push_back({lines[i], static_cast<int>(i) * 40, 30, 90});
    }
    return result;
}

// A scorer that always fails, like an unreachable model server.
class BrokenScorer : public LineScorer {
public:
    Result<std::vector<double>> scoreLines(const std::vector<std::string> &) override {
        return fail(ErrorCode::Network, "model server unreachable");
    }
    ExtractionSource source() const override { return ExtractionSource::Embedding; }
};

struct Fixture {
    KeywordLineScorer keywords;
    testing::RecordingLogger logger;
};

} // namespace

TEST_CASE("the TOTAL line wins over items, tax and the cash given") {
    Fixture fixture;
    PriceExtractor extractor(fixture.keywords, fixture.keywords, nullptr, fixture.logger);

    const auto result = extractor.extract(page({
        "REWE Markt",
        "13.10.2026",
        "Milch          1,29",
        "Brot           2,49",
        "SUMME EUR      3,78",
        "MwSt 7%        0,25",
        "Gegeben Bar   10,00",
        "Rückgeld       6,22",
    }));

    REQUIRE(result);
    REQUIRE(result->total);
    CHECK(result->total->amountMinor == 378);
    CHECK(result->total->currency == "EUR");
    CHECK(result->purchasedAt == "2026-10-13T12:00:00.000Z");
    CHECK(result->merchant == "REWE Markt");
    CHECK(result->source == ExtractionSource::Keywords);
    CHECK(result->confidence > 0.6);
}

TEST_CASE("Russian receipts work the same way") {
    Fixture fixture;
    PriceExtractor extractor(fixture.keywords, fixture.keywords, nullptr, fixture.logger);
    const auto result = extractor.extract(
        page({"Пятёрочка", "Хлеб  45,90", "Молоко 89,99", "ИТОГО ₽ 135,89", "Наличные 200,00", "Сдача 64,11"}));
    REQUIRE(result->total);
    CHECK(result->total->amountMinor == 13589);
    CHECK(result->total->currency == "RUB");
}

TEST_CASE("without any label, the largest amount near the bottom is chosen, with low confidence") {
    Fixture fixture;
    PriceExtractor extractor(fixture.keywords, fixture.keywords, nullptr, fixture.logger);
    const auto result = extractor.extract(page({"Shop", "A 1.00", "B 2.00", "3.00"}));
    REQUIRE(result->total);
    CHECK(result->total->amountMinor == 300);
    CHECK(result->confidence < 0.6);
}

TEST_CASE("the default currency fills in when the receipt prints none") {
    Fixture fixture;
    PriceExtractor extractor(fixture.keywords, fixture.keywords, nullptr, fixture.logger,
                             {.askLlmBelowConfidence = 0.6, .defaultCurrency = "GBP"});
    CHECK(extractor.extract(page({"Total 9.99"}))->total->currency == "GBP");
    CHECK(extractor.extract(page({"Total 9.99 EUR"}))->total->currency == "EUR");
}

TEST_CASE("a receipt without amounts yields no total instead of a guess") {
    Fixture fixture;
    PriceExtractor extractor(fixture.keywords, fixture.keywords, nullptr, fixture.logger);
    const auto result = extractor.extract(page({"Thank you", "for shopping"}));
    REQUIRE(result);
    CHECK_FALSE(result->total);
    CHECK(result->confidence == 0);
}

TEST_CASE("a failing scorer falls back to keywords and logs why") {
    Fixture fixture;
    BrokenScorer broken;
    PriceExtractor extractor(broken, fixture.keywords, nullptr, fixture.logger);
    const auto result = extractor.extract(page({"Total 9.99"}));
    REQUIRE(result->total);
    CHECK(result->source == ExtractionSource::Keywords);
    CHECK(fixture.logger.messages.size() == 1);
}

TEST_CASE("an unsure result asks the LLM, which may only pick a printed amount") {
    Fixture fixture;
    testing::FakeCompletionEngine llmEngine(R"({"total": "2.00", "currency": "USD"})");
    LlmTotalExtractor llm(llmEngine);
    PriceExtractor extractor(fixture.keywords, fixture.keywords, &llm, fixture.logger);

    const auto result = extractor.extract(page({"Shop", "A 1.00", "B 2.00", "3.00"}));
    REQUIRE(result->total);
    CHECK(result->total->amountMinor == 200);
    CHECK(result->source == ExtractionSource::Llm);
    CHECK(llmEngine.lastInput.find("B 2.00") != std::string::npos);
}

TEST_CASE("a confident result does not ask the LLM") {
    Fixture fixture;
    testing::FakeCompletionEngine llmEngine(R"({"total": "1.00", "currency": "EUR"})");
    LlmTotalExtractor llm(llmEngine);
    PriceExtractor extractor(fixture.keywords, fixture.keywords, &llm, fixture.logger);

    const auto result = extractor.extract(page({"Milk 1.00", "TOTAL 1.00", "Cash 5.00", "Change 4.00"}));
    CHECK(result->source == ExtractionSource::Keywords);
    CHECK(llmEngine.lastInput.empty());
}
