#pragma once

// The no-model fallback: literal phrase matching against TotalAnchors.
// Fast and predictable, but blind to wording it doesn't list.

#include "extract/LineScorer.hpp"

namespace receipts::extract {

class KeywordLineScorer : public LineScorer {
public:
    Result<std::vector<double>> scoreLines(const std::vector<std::string> &lines) override;
    ExtractionSource source() const override { return ExtractionSource::Keywords; }
};

} // namespace receipts::extract
