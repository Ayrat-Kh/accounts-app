#pragma once

#include <chrono>

namespace accounts::shared
{
    using Datetime = std::chrono::time_point<std::chrono::system_clock>;
}