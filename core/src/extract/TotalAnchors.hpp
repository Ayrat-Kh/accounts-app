#pragma once

// How receipts label the amount to pay, the amounts that are *not* it, and
// ordinary purchased items, in the three languages this app reads.
// KeywordLineScorer looks for the first two lists literally; EmbeddingLineScorer
// compares meanings with all three, so "Betrag EUR" or "Amount payable"
// still score as totals while "Rueckgeld" or "Vollkornbrot" do not.

#include <string>
#include <vector>

namespace receipts::extract {

inline const std::vector<std::string> &totalPhrases() {
    static const std::vector<std::string> phrases = {
        "total",     "grand total", "amount due", "total to pay", "balance due", "summe", "gesamt",
        "zu zahlen", "endbetrag",   "итого",      "итог",         "к оплате",    "всего", "сумма к оплате",
    };
    return phrases;
}

inline const std::vector<std::string> &notTotalPhrases() {
    static const std::vector<std::string> phrases = {
        "subtotal", "sub total", "tax",     "vat",           "change", "cash",  "tip",      "discount", "mwst",
        "netto",    "rückgeld",  "gegeben", "zwischensumme", "ндс",    "сдача", "наличные", "скидка",   "получено",
    };
    return phrases;
}

// Typical product lines. Without them, an item like "Milch" can sit as close
// to "total" as to "tax", because short receipt words all look alike to an
// embedding model; with them, items have a better home than "total".
inline const std::vector<std::string> &itemPhrases() {
    static const std::vector<std::string> phrases = {
        "milk",  "bread", "coffee", "bananas",  "cheese", "pizza", "shampoo", "water", "milch",  "brot",   "kaffee",
        "äpfel", "käse",  "wein",   "brötchen", "молоко", "хлеб",  "кофе",    "сыр",   "яблоки", "творог", "кефир",
    };
    return phrases;
}

} // namespace receipts::extract
