#pragma once

#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "accounts/shared/types.hpp"

namespace accounts
{
    std::string timePointToString(const Datetime &timePoint);

    Datetime msToTimePoint(std::int64_t ms);

    Datetime convertToTimePoint(const std::string &timeStr);
}