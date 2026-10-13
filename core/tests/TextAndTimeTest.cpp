#include "util/Text.hpp"
#include "util/Time.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace receipts;

TEST_CASE("lower-casing covers ASCII, German umlauts and Cyrillic") {
    CHECK(text::toLower("TOTAL") == "total");
    CHECK(text::toLower("RÜCKGELD") == "rückgeld");
    CHECK(text::toLower("ИТОГО ЁЖ") == "итого ёж");
}

TEST_CASE("numbers are stripped so only the words of a line remain") {
    CHECK(text::withoutNumbers("TOTAL   12,34 EUR") == "TOTAL EUR");
    CHECK(text::withoutNumbers("2 x 1,49   2,98") == "x");
}

TEST_CASE("RFC 3339 formatting is exact, including before 1970") {
    CHECK(time::formatRfc3339(0) == "1970-01-01T00:00:00.000Z");
    CHECK(time::formatRfc3339(1'760'347'800'123) == "2025-10-13T09:30:00.123Z");
    CHECK(time::formatRfc3339(-1) == "1969-12-31T23:59:59.999Z");
}

TEST_CASE("a receipt date becomes noon UTC that day") {
    CHECK(time::dateToRfc3339Noon("2026-10-13") == "2026-10-13T12:00:00.000Z");
    CHECK(time::dateToRfc3339Noon("2028-02-29") == "2028-02-29T12:00:00.000Z");
    CHECK_FALSE(time::dateToRfc3339Noon("2026-02-29"));
    CHECK_FALSE(time::dateToRfc3339Noon("13.10.2026"));
}
