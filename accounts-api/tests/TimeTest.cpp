#include "util/Time.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace accounts;

TEST_CASE("timestamps are formatted as RFC 3339 UTC with milliseconds") {
    const auto timestamp = parseRfc3339("2026-10-13T09:30:00.250Z").value();
    CHECK(formatRfc3339(timestamp) == "2026-10-13T09:30:00.250Z");
}

TEST_CASE("RFC 3339 parsing accepts Z, offsets and missing fractions") {
    CHECK(parseRfc3339("2026-10-13T09:30:00Z"));
    CHECK(parseRfc3339("2026-10-13T11:30:00+02:00") == parseRfc3339("2026-10-13T09:30:00Z"));
}

TEST_CASE("RFC 3339 parsing rejects anything else") {
    CHECK_FALSE(parseRfc3339(""));
    CHECK_FALSE(parseRfc3339("13.10.2026"));
    CHECK_FALSE(parseRfc3339("2026-10-13"));
    CHECK_FALSE(parseRfc3339("2026-10-13T09:30:00Z trailing"));
}
