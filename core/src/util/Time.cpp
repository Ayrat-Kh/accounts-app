#include "util/Time.hpp"

#include <chrono>
#include <cstdio>

namespace receipts::time {

namespace {

bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

int daysInMonth(int year, int month) {
    static constexpr int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return month == 2 && isLeapYear(year) ? 29 : days[month - 1];
}

struct CivilDate {
    int year;
    int month;
    int day;
};

CivilDate civilFromDays(std::int64_t days) {
    days += 719468;
    const std::int64_t era = (days >= 0 ? days : days - 146096) / 146097;
    const auto dayOfEra = static_cast<unsigned>(days - era * 146097);
    const unsigned yearOfEra = (dayOfEra - dayOfEra / 1460 + dayOfEra / 36524 - dayOfEra / 146096) / 365;
    const unsigned dayOfYear = dayOfEra - (365 * yearOfEra + yearOfEra / 4 - yearOfEra / 100);
    const unsigned monthPrime = (5 * dayOfYear + 2) / 153;
    const auto day = static_cast<int>(dayOfYear - (153 * monthPrime + 2) / 5 + 1);
    const auto month = static_cast<int>(monthPrime < 10 ? monthPrime + 3 : monthPrime - 9);
    const auto year = static_cast<int>(yearOfEra + era * 400 + (month <= 2 ? 1 : 0));
    return {year, month, day};
}

} // namespace

std::int64_t nowUnixMillis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

std::optional<std::int64_t> daysFromCivil(int year, int month, int day) {
    if (month < 1 || month > 12 || day < 1 || day > daysInMonth(year, month)) {
        return std::nullopt;
    }
    year -= month <= 2 ? 1 : 0;
    const std::int64_t era = (year >= 0 ? year : year - 399) / 400;
    const auto yearOfEra = static_cast<unsigned>(year - era * 400);
    const unsigned dayOfYear = (153 * (month > 2 ? month - 3 : month + 9) + 2) / 5 + day - 1;
    const unsigned dayOfEra = yearOfEra * 365 + yearOfEra / 4 - yearOfEra / 100 + dayOfYear;
    return era * 146097 + static_cast<std::int64_t>(dayOfEra) - 719468;
}

std::string formatRfc3339(std::int64_t unixMillis) {
    constexpr std::int64_t millisPerDay = 86'400'000;
    std::int64_t days = unixMillis / millisPerDay;
    std::int64_t millisOfDay = unixMillis % millisPerDay;
    if (millisOfDay < 0) {
        millisOfDay += millisPerDay;
        --days;
    }
    const CivilDate date = civilFromDays(days);
    char buffer[32];
    std::snprintf(buffer, sizeof buffer, "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ", date.year, date.month, date.day,
                  static_cast<int>(millisOfDay / 3'600'000), static_cast<int>(millisOfDay / 60'000 % 60),
                  static_cast<int>(millisOfDay / 1000 % 60), static_cast<int>(millisOfDay % 1000));
    return buffer;
}

std::optional<std::string> dateToRfc3339Noon(std::string_view isoDate) {
    int year = 0;
    int month = 0;
    int day = 0;
    char trailing = 0;
    if (std::sscanf(std::string(isoDate).c_str(), "%4d-%2d-%2d%c", &year, &month, &day, &trailing) != 3) {
        return std::nullopt;
    }
    const auto days = daysFromCivil(year, month, day);
    if (!days) {
        return std::nullopt;
    }
    return formatRfc3339(*days * 86'400'000 + 12 * 3'600'000);
}

} // namespace receipts::time
