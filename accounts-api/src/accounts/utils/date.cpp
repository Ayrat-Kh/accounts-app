#include "date.hpp"

std::string accounts::timePointToString(const Datetime &timePoint)
{
    auto timeT = std::chrono::system_clock::to_time_t(timePoint);

    boost::posix_time::ptime pt = boost::posix_time::from_time_t(timeT);
    return boost::posix_time::to_iso_extended_string(pt);
}

accounts::Datetime accounts::msToTimePoint(std::int64_t ms)
{
    return std::chrono::system_clock::from_time_t(0) + std::chrono::milliseconds(ms);
}

accounts::Datetime accounts::convertToTimePoint(const std::string &timeStr)
{

    boost::posix_time::ptime pt = boost::posix_time::from_iso_extended_string(timeStr);
    boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
    boost::posix_time::time_duration diff = pt - epoch;
    return std::chrono::system_clock::from_time_t(diff.total_seconds());
}
