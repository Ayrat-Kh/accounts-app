#include "extract/ReceiptFields.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace receipts::extract;

TEST_CASE("the purchase date is found in common formats") {
    CHECK(findPurchaseDate({"Kasse 3", "13.10.2026 09:31"}) == "2026-10-13");
    CHECK(findPurchaseDate({"2026-10-13T09:31"}) == "2026-10-13");
    CHECK(findPurchaseDate({"13/10/26"}) == "2026-10-13");
}

TEST_CASE("slash dates are read day-first unless that is impossible") {
    CHECK(findPurchaseDate({"05/10/2026"}) == "2026-10-05");
    CHECK(findPurchaseDate({"10/13/2026"}) == "2026-10-13");
}

TEST_CASE("impossible dates are skipped") {
    CHECK(findPurchaseDate({"31.02.2026", "01.03.2026"}) == "2026-03-01");
    CHECK_FALSE(findPurchaseDate({"no dates here"}));
}

TEST_CASE("the merchant is the first wordy line near the top") {
    CHECK(guessMerchant({"*** 12345 ***", "LIDL Berlin", "Milk 1,29"}) == "LIDL Berlin");
    CHECK(guessMerchant({"ВкусВилл", "Итого 120,00"}) == "ВкусВилл");
    CHECK_FALSE(guessMerchant({"12,34", "56,78"}));
}
