#include "extract/PriceExtractor.hpp"

#include "extract/AmountParser.hpp"
#include "extract/ReceiptFields.hpp"
#include "util/Text.hpp"
#include "util/Time.hpp"

#include <algorithm>
#include <map>

namespace receipts::extract {

namespace {

constexpr double meaningWeight = 0.6;
constexpr double positionWeight = 0.2;
constexpr double sizeWeight = 0.2;
// A generative model's answer is checked against the printed amounts, so
// when it agrees with one, we trust it at least this much.
constexpr double llmAgreementConfidence = 0.75;

std::vector<std::string> lineTexts(const ocr::OcrPage &page) {
    std::vector<std::string> texts;
    texts.reserve(page.lines.size());
    for (const auto &line : page.lines) {
        texts.push_back(line.text);
    }
    return texts;
}

// The currency most lines mention: receipts often print it once, in a header or on the total line.
std::string receiptCurrency(const std::vector<std::string> &lines) {
    std::map<std::string, int> counts;
    for (const auto &line : lines) {
        if (auto currency = findCurrency(line)) {
            ++counts[*currency];
        }
    }
    const auto mostCommon = std::ranges::max_element(counts, {}, [](const auto &entry) { return entry.second; });
    return mostCommon == counts.end() ? std::string{} : mostCommon->first;
}

double relativePosition(const ocr::OcrPage &page, std::size_t lineIndex) {
    if (page.imageHeight > 0) {
        return std::clamp(static_cast<double>(page.lines[lineIndex].top) / page.imageHeight, 0.0, 1.0);
    }
    return page.lines.size() > 1 ? static_cast<double>(lineIndex) / static_cast<double>(page.lines.size() - 1) : 1.0;
}

} // namespace

PriceExtractor::PriceExtractor(LineScorer &scorer, LineScorer &fallbackScorer, LlmTotalExtractor *llm, Logger &logger,
                               PriceExtractorOptions options)
    : scorer_(scorer), fallbackScorer_(fallbackScorer), llm_(llm), logger_(logger), options_(options) {}

std::vector<PriceExtractor::Candidate> PriceExtractor::findCandidates(const ocr::OcrPage &page) const {
    std::string defaultCurrency = receiptCurrency(lineTexts(page));
    if (defaultCurrency.empty()) {
        defaultCurrency = options_.defaultCurrency;
    }
    std::vector<Candidate> candidates;
    for (std::size_t i = 0; i < page.lines.size(); ++i) {
        const std::string &line = page.lines[i].text;
        const std::string currency = findCurrency(line).value_or(defaultCurrency);
        for (const ParsedAmount &amount : findAmounts(line)) {
            candidates.push_back({Money{amount.amountMinor, currency}, i});
        }
    }
    return candidates;
}

Result<ExtractionSource> PriceExtractor::scoreCandidates(const ocr::OcrPage &page, std::vector<Candidate> &candidates) {
    // Score each line once, by its words only.
    std::vector<std::string> labels;
    for (const auto &candidate : candidates) {
        labels.push_back(text::withoutNumbers(page.lines[candidate.lineIndex].text));
    }

    LineScorer *scorer = &scorer_;
    auto meanings = scorer->scoreLines(labels);
    if (!meanings) {
        logger_.log(LogLevel::Warning, "line scorer failed, using keywords: " + meanings.error().message);
        scorer = &fallbackScorer_;
        meanings = scorer->scoreLines(labels);
        if (!meanings) {
            return std::unexpected(meanings.error());
        }
    }

    const auto largest = std::ranges::max_element(candidates, {}, [](const Candidate &c) {
                             return c.money.amountMinor;
                         })->money.amountMinor;
    for (std::size_t i = 0; i < candidates.size(); ++i) {
        const double size = largest > 0 ? static_cast<double>(candidates[i].money.amountMinor) / largest : 0;
        candidates[i].meaning = (*meanings)[i];
        candidates[i].score = meaningWeight * candidates[i].meaning +
                              positionWeight * relativePosition(page, candidates[i].lineIndex) + sizeWeight * size;
    }
    std::ranges::stable_sort(candidates, std::greater{}, &Candidate::score);
    return scorer->source();
}

void PriceExtractor::askLlmIfUnsure(const std::vector<std::string> &lines, const std::vector<Candidate> &candidates,
                                    ScanResult &result) {
    if (llm_ == nullptr || result.confidence >= options_.askLlmBelowConfidence) {
        return;
    }
    std::vector<Money> printedAmounts;
    for (const auto &candidate : candidates) {
        printedAmounts.push_back(candidate.money);
    }
    auto answer = llm_->extractTotal(lines, printedAmounts);
    if (!answer) {
        logger_.log(LogLevel::Warning, "LLM fallback failed: " + answer.error().message);
        return;
    }
    if (*answer) {
        result.total = **answer;
        result.source = ExtractionSource::Llm;
        result.confidence = std::max(result.confidence, llmAgreementConfidence);
    }
}

Result<ScanResult> PriceExtractor::extract(const ocr::OcrPage &page) {
    ScanResult result;
    result.ocrLines = lineTexts(page);
    if (const auto date = findPurchaseDate(result.ocrLines)) {
        result.purchasedAt = time::dateToRfc3339Noon(*date);
    }
    result.merchant = guessMerchant(result.ocrLines);

    std::vector<Candidate> candidates = findCandidates(page);
    if (candidates.empty()) {
        return result; // no amounts at all: let the user type it in
    }
    auto source = scoreCandidates(page, candidates);
    if (!source) {
        return std::unexpected(source.error());
    }

    const Candidate &best = candidates.front();
    const auto runnerUp = std::ranges::find_if(
        candidates, [&](const Candidate &c) { return c.money.amountMinor != best.money.amountMinor; });
    const double margin = runnerUp == candidates.end() ? best.score : best.score - runnerUp->score;

    result.total = best.money;
    result.source = *source;
    result.confidence = 0.5 * std::clamp(best.meaning, 0.0, 1.0) + 0.5 * std::clamp(margin * 2, 0.0, 1.0);
    for (const auto &candidate : candidates) {
        result.candidates.push_back({candidate.money, page.lines[candidate.lineIndex].text, candidate.score});
    }

    askLlmIfUnsure(result.ocrLines, candidates, result);
    return result;
}

} // namespace receipts::extract
