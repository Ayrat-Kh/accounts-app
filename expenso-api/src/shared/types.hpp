#pragma once

#include <chrono>

namespace app::shared
{
    using Datetime = std::chrono::time_point<std::chrono::system_clock>;
}