#pragma once

// OCR lines -> ScanResult: which printed amount is the total, how sure we
// are, plus the date and merchant.
//
// For every amount on the receipt:
//
//   score = 0.6 * meaning     how much its line reads like "total" (LineScorer, -1..1)
//         + 0.2 * position    totals sit near the bottom (0 top .. 1 bottom)
//         + 0.2 * size        totals are the largest amount, apart from cash given
//                             and change, which the meaning term pushes down
//
// confidence = half "the best line clearly reads like a total" + half "it
// clearly beats the best *different* amount". (The same amount printed
// twice, e.g. on the TOTAL line and the CARD line, agrees with itself, so it
// doesn't lower confidence.)
//
// Below `askLlmBelowConfidence`, an optional LlmTotalExtractor gets a say.

#include "extract/LineScorer.hpp"
#include "extract/LlmTotalExtractor.hpp"
#include "ocr/OcrEngine.hpp"
#include "receipts/Ports.hpp"

namespace receipts::extract {

struct PriceExtractorOptions {
    double askLlmBelowConfidence = 0.6;
    // Used when the receipt prints no currency at all; empty: leave it to the user.
    std::string defaultCurrency;
};

class PriceExtractor {
public:
    // `scorer` is the preferred scorer (embedding); `fallbackScorer` (keywords)
    // is used when it fails, e.g. because a model server is unreachable.
    // `llm` may be null.
    PriceExtractor(LineScorer &scorer, LineScorer &fallbackScorer, LlmTotalExtractor *llm, Logger &logger,
                   PriceExtractorOptions options = {});

    Result<ScanResult> extract(const ocr::OcrPage &page);

private:
    struct Candidate {
        Money money;
        std::size_t lineIndex;
        double meaning = 0; // the LineScorer's verdict on this amount's line
        double score = 0;   // the combined score above
    };

    std::vector<Candidate> findCandidates(const ocr::OcrPage &page) const;
    Result<ExtractionSource> scoreCandidates(const ocr::OcrPage &page, std::vector<Candidate> &candidates);
    void askLlmIfUnsure(const std::vector<std::string> &lines, const std::vector<Candidate> &candidates,
                        ScanResult &result);

    LineScorer &scorer_;
    LineScorer &fallbackScorer_;
    LlmTotalExtractor *llm_;
    Logger &logger_;
    PriceExtractorOptions options_;
};

} // namespace receipts::extract
