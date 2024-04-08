#pragma once

#include <string>

#include "shared/types.hpp"

namespace app::utils
{
    std::string timePointToString(const app::shared::Datetime &timePoint);

    app::shared::Datetime msToTimePoint(std::int64_t ms);

}