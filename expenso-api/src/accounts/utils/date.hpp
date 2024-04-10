#pragma once

#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "accounts/shared/types.hpp"

namespace accounts::utils
{
    std::string timePointToString(const ::accounts::shared::Datetime &timePoint);

    ::accounts::shared::Datetime msToTimePoint(std::int64_t ms);

    ::accounts::shared::Datetime convertToTimePoint(const std::string &timeStr);
}