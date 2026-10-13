#include "extract/LlmTotalExtractor.hpp"
#include "support/Fakes.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace receipts;
using namespace receipts::extract;

namespace {

const std::vector<Money> printed = {{129, "EUR"}, {378, "EUR"}};

std::optional<Money> askWith(const std::string &reply) {
    testing::FakeCompletionEngine engine(reply);
    LlmTotalExtractor extractor(engine);
    return extractor.extractTotal({"Milk 1,29", "Total 3,78"}, printed).value();
}

} // namespace

TEST_CASE("the model's answer is accepted when it is printed on the receipt") {
    const auto total = askWith(R"({"total": "3.78", "currency": "EUR"})");
    REQUIRE(total);
    CHECK(total->amountMinor == 378);
}

TEST_CASE("an invented amount is rejected") {
    CHECK_FALSE(askWith(R"({"total": "4.20", "currency": "EUR"})"));
}

TEST_CASE("malformed or empty answers are rejected") {
    CHECK_FALSE(askWith("not json"));
    CHECK_FALSE(askWith(R"({"total": null, "currency": null})"));
    CHECK_FALSE(askWith(R"({"total": "3,78"})"));
}

TEST_CASE("decimal amounts parse to minor units") {
    CHECK(parseDecimalAmount("12.34") == 1234);
    CHECK(parseDecimalAmount("12") == 1200);
    CHECK_FALSE(parseDecimalAmount("12.3"));
    CHECK_FALSE(parseDecimalAmount("12.345"));
    CHECK_FALSE(parseDecimalAmount("-1.00"));
    CHECK_FALSE(parseDecimalAmount("abc"));
}
