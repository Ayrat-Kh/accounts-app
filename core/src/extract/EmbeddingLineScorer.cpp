#include "extract/EmbeddingLineScorer.hpp"

#include "extract/TotalAnchors.hpp"
#include "inference/VectorMath.hpp"

#include <algorithm>

namespace receipts::extract {

namespace {

// A clear verdict shows up as a similarity difference of about 0.1 (measured
// on testdata/receipts); the stretch maps that to a confident score of ~1.
constexpr double stretch = 8.0;

float bestSimilarity(const inference::Embedding &line, const std::vector<inference::Embedding> &anchors) {
    float best = -1;
    for (const auto &anchor : anchors) {
        best = std::max(best, inference::cosineSimilarity(line, anchor));
    }
    return best;
}

} // namespace

EmbeddingLineScorer::EmbeddingLineScorer(inference::EmbeddingEngine &engine, std::string textPrefix)
    : engine_(engine), textPrefix_(std::move(textPrefix)) {}

Result<std::vector<inference::Embedding>> EmbeddingLineScorer::embedWithPrefix(const std::vector<std::string> &texts) {
    std::vector<std::string> prefixed;
    prefixed.reserve(texts.size());
    for (const std::string &text : texts) {
        prefixed.push_back(textPrefix_ + text);
    }
    return engine_.embed(prefixed);
}

Result<const EmbeddingLineScorer::Anchors *> EmbeddingLineScorer::anchors() {
    std::lock_guard lock(anchorsMutex_);
    if (!anchors_) {
        auto total = embedWithPrefix(totalPhrases());
        if (!total) {
            return std::unexpected(total.error());
        }
        auto notTotal = embedWithPrefix(notTotalPhrases());
        if (!notTotal) {
            return std::unexpected(notTotal.error());
        }
        auto items = embedWithPrefix(itemPhrases());
        if (!items) {
            return std::unexpected(items.error());
        }
        anchors_ = Anchors{std::move(*total), std::move(*notTotal), std::move(*items)};
    }
    return &*anchors_;
}

Result<std::vector<double>> EmbeddingLineScorer::scoreLines(const std::vector<std::string> &lines) {
    auto anchorVectors = anchors();
    if (!anchorVectors) {
        return std::unexpected(anchorVectors.error());
    }
    auto lineVectors = embedWithPrefix(lines);
    if (!lineVectors) {
        return std::unexpected(lineVectors.error());
    }

    std::vector<double> scores;
    scores.reserve(lines.size());
    for (const auto &line : *lineVectors) {
        const Anchors &anchors = **anchorVectors;
        const double closestOther =
            std::max(bestSimilarity(line, anchors.notTotal), bestSimilarity(line, anchors.items));
        const double difference = bestSimilarity(line, anchors.total) - closestOther;
        scores.push_back(std::clamp(difference * stretch, -1.0, 1.0));
    }
    return scores;
}

} // namespace receipts::extract
