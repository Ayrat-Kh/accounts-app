#pragma once

// Scores lines by meaning with an embedding model (EmbeddingGemma), as a
// nearest-prototype classifier with three classes:
//
//   score(line) = similarity to the closest "total" phrase
//               - similarity to the closest "other amount" or "item" phrase
//
// So "Betrag EUR" lands near "total to pay" even though no keyword matches,
// while "Zwischensumme" (near "subtotal") and "Vollkornbrot" (near "bread")
// are pushed down. Only the difference matters: all short receipt words are
// fairly similar to each other in absolute terms. It is stretched and
// clamped to -1..1, KeywordLineScorer's scale.
//
// The anchor phrases are embedded once and cached.

#include "extract/LineScorer.hpp"
#include "inference/EmbeddingEngine.hpp"

#include <mutex>
#include <optional>

namespace receipts::extract {

class EmbeddingLineScorer : public LineScorer {
public:
    EmbeddingLineScorer(inference::EmbeddingEngine &engine, std::string textPrefix);

    Result<std::vector<double>> scoreLines(const std::vector<std::string> &lines) override;
    ExtractionSource source() const override { return ExtractionSource::Embedding; }

private:
    struct Anchors {
        std::vector<inference::Embedding> total;
        std::vector<inference::Embedding> notTotal;
        std::vector<inference::Embedding> items;
    };

    Result<const Anchors *> anchors();
    Result<std::vector<inference::Embedding>> embedWithPrefix(const std::vector<std::string> &texts);

    inference::EmbeddingEngine &engine_;
    std::string textPrefix_;
    std::optional<Anchors> anchors_;
    std::mutex anchorsMutex_;
};

} // namespace receipts::extract
