#include "date.hpp"

#include <boost/format.hpp>

#include <iostream>

std::string app::utils::timePointToString(const std::chrono::system_clock::time_point &timePoint)
{
    auto timeT = std::chrono::system_clock::to_time_t(timePoint);

    auto itt = (*std::localtime(&timeT));

    // Format as ISO 8601 string
    std::string iso8601 = (boost::format("%04d-%02d-%02dT%02d:%02d:%02dZ") % (itt.tm_year + 1900) % (itt.tm_mon + 1) % itt.tm_mday % itt.tm_hour % itt.tm_min % itt.tm_sec).str();

    return iso8601;
}

std::chrono::system_clock::time_point app::utils::msToTimePoint(std::int64_t ms)
{
    return std::chrono::system_clock::from_time_t(0) + std::chrono::milliseconds(ms);
}
