#pragma once

#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "shared/types.hpp"

namespace app::utils
{
    std::string timePointToString(const app::shared::Datetime &timePoint);

    app::shared::Datetime msToTimePoint(std::int64_t ms);

    app::shared::Datetime convertToTimePoint(const std::string &timeStr);
}