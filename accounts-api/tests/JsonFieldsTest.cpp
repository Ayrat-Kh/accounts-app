#include "http/JsonFields.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace accounts;
using accounts::http::JsonFields;
using accounts::http::parseJsonObject;

TEST_CASE("parseJsonObject accepts only JSON objects") {
    CHECK(parseJsonObject(R"({"a": 1})"));
    CHECK(parseJsonObject("[1, 2]").error().code == ErrorCode::InvalidInput);
    CHECK(parseJsonObject("{not json").error().code == ErrorCode::InvalidInput);
}

TEST_CASE("JsonFields reads present fields of the right type") {
    const auto body = parseJsonObject(R"({"name": "Lidl", "count": 3, "ratio": 0.5})").value();
    JsonFields fields(body);
    CHECK(fields.requiredString("name") == "Lidl");
    CHECK(fields.optionalInt64("count") == 3);
    CHECK(fields.optionalDouble("ratio") == 0.5);
    CHECK_FALSE(fields.firstError());
}

TEST_CASE("JsonFields treats missing and null optional fields as absent") {
    const auto body = parseJsonObject(R"({"merchant": null})").value();
    JsonFields fields(body);
    CHECK_FALSE(fields.optionalString("merchant"));
    CHECK_FALSE(fields.optionalString("missing"));
    CHECK_FALSE(fields.firstError());
}

TEST_CASE("JsonFields reports the first missing or mistyped field") {
    const auto body = parseJsonObject(R"({"amount_minor": "12", "currency": 5})").value();
    JsonFields fields(body);
    fields.requiredInt64("amount_minor");
    fields.requiredString("currency");
    REQUIRE(fields.firstError());
    CHECK(fields.firstError()->message == "field 'amount_minor' must be an integer");
}
