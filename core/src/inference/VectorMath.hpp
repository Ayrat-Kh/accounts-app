#pragma once

#include "inference/EmbeddingEngine.hpp"

#include <cmath>
#include <numeric>

namespace receipts::inference {

inline void normalize(Embedding &vector) {
    const float length = std::sqrt(std::inner_product(vector.begin(), vector.end(), vector.begin(), 0.0f));
    if (length > 0) {
        for (float &value : vector) {
            value /= length;
        }
    }
}

// For normalised vectors: 1 = same meaning, 0 = unrelated.
inline float cosineSimilarity(const Embedding &a, const Embedding &b) {
    if (a.size() != b.size()) {
        return 0;
    }
    return std::inner_product(a.begin(), a.end(), b.begin(), 0.0f);
}

} // namespace receipts::inference
