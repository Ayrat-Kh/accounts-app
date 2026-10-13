#include "extract/AmountParser.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace receipts::extract;

namespace {

std::vector<std::int64_t> amountsIn(std::string_view line) {
    std::vector<std::int64_t> amounts;
    for (const auto &amount : findAmounts(line)) {
        amounts.push_back(amount.amountMinor);
    }
    return amounts;
}

} // namespace

TEST_CASE("amounts use either decimal separator") {
    CHECK(amountsIn("Milk 1,29") == std::vector<std::int64_t>{129});
    CHECK(amountsIn("Milk 1.29") == std::vector<std::int64_t>{129});
}

TEST_CASE("thousands separators are understood in both conventions") {
    CHECK(amountsIn("TOTAL 1.234,56") == std::vector<std::int64_t>{123456});
    CHECK(amountsIn("TOTAL 1,234.56") == std::vector<std::int64_t>{123456});
    CHECK(amountsIn("ИТОГО 1 234,56") == std::vector<std::int64_t>{123456});
}

TEST_CASE("numbers that are not prices are ignored") {
    CHECK(amountsIn("2 x").empty());                // quantity
    CHECK(amountsIn("Bananas 0,512 kg").empty());   // weight: three decimals
    CHECK(amountsIn("Art. 4006381333931").empty()); // article number
    CHECK(amountsIn("Discount -1,50").empty());     // negative: never the total
    CHECK(amountsIn("13.10.2026 18:42").empty());   // a date, not 13.10
}

TEST_CASE("several amounts on one line are all found") {
    CHECK(amountsIn("2 x 1,49   2,98") == std::vector<std::int64_t>{149, 298});
}

TEST_CASE("currencies are detected by symbol or code, case-insensitively") {
    CHECK(findCurrency("SUMME EUR 12,34") == "EUR");
    CHECK(findCurrency("Total: $12.34") == "USD");
    CHECK(findCurrency("12,34 €") == "EUR");
    CHECK(findCurrency("Итого 120,00 руб") == "RUB");
    CHECK(findCurrency("Milk 1,29") == std::nullopt);
}
