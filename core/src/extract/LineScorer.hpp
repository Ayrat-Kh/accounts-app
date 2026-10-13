#pragma once

// Answers "how much does this receipt line read like the total to pay?" for
// a batch of lines, each in -1..1 (1: certainly the total, -1: certainly
// something else, e.g. tax or change). Lines are passed without their
// numbers (text::withoutNumbers).

#include "receipts/Result.hpp"
#include "receipts/Types.hpp"

#include <string>
#include <vector>

namespace receipts::extract {

class LineScorer {
public:
    virtual ~LineScorer() = default;
    virtual Result<std::vector<double>> scoreLines(const std::vector<std::string> &lines) = 0;
    virtual ExtractionSource source() const = 0;
};

} // namespace receipts::extract
