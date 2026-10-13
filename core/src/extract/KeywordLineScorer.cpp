#include "extract/KeywordLineScorer.hpp"

#include "extract/TotalAnchors.hpp"
#include "util/Text.hpp"

#include <algorithm>

namespace receipts::extract {

namespace {

bool containsAny(const std::string &line, const std::vector<std::string> &phrases) {
    return std::ranges::any_of(phrases, [&](const std::string &phrase) { return line.find(phrase) != std::string::npos; });
}

double scoreLine(const std::string &line) {
    const std::string lower = text::toLower(line);
    // "Not total" first: "subtotal" contains "total".
    if (containsAny(lower, notTotalPhrases())) {
        return -1;
    }
    if (containsAny(lower, totalPhrases())) {
        return 1;
    }
    return 0;
}

} // namespace

Result<std::vector<double>> KeywordLineScorer::scoreLines(const std::vector<std::string> &lines) {
    std::vector<double> scores;
    scores.reserve(lines.size());
    std::ranges::transform(lines, std::back_inserter(scores), scoreLine);
    return scores;
}

} // namespace receipts::extract
