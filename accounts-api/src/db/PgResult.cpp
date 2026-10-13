#include "db/PgResult.hpp"

#include <charconv>
#include <stdexcept>

namespace accounts::db {

namespace {

template <class Number>
Number parseNumber(std::string_view text, std::string_view column) {
    Number value{};
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (error != std::errc{} || end != text.data() + text.size()) {
        throw std::runtime_error("column " + std::string(column) + " is not a number: " + std::string(text));
    }
    return value;
}

} // namespace

int PgRow::columnIndex(std::string_view column) const {
    const int index = PQfnumber(result_, std::string(column).c_str());
    if (index < 0) {
        // A typo in a column name is a programming error, not a runtime condition.
        throw std::logic_error("no such column in result: " + std::string(column));
    }
    return index;
}

bool PgRow::isNull(std::string_view column) const {
    return PQgetisnull(result_, row_, columnIndex(column)) == 1;
}

std::string PgRow::text(std::string_view column) const {
    const int index = columnIndex(column);
    return {PQgetvalue(result_, row_, index), static_cast<std::size_t>(PQgetlength(result_, row_, index))};
}

std::optional<std::string> PgRow::optionalText(std::string_view column) const {
    if (isNull(column)) {
        return std::nullopt;
    }
    return text(column);
}

std::int64_t PgRow::int64(std::string_view column) const {
    return parseNumber<std::int64_t>(text(column), column);
}

std::optional<std::int64_t> PgRow::optionalInt64(std::string_view column) const {
    if (isNull(column)) {
        return std::nullopt;
    }
    return int64(column);
}

std::optional<double> PgRow::optionalDouble(std::string_view column) const {
    if (isNull(column)) {
        return std::nullopt;
    }
    return parseNumber<double>(text(column), column);
}

} // namespace accounts::db
