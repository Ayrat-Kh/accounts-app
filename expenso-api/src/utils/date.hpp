#pragma once

#include <chrono>
#include <string>

namespace app
{
    namespace utils
    {
        std::string timePointToString(const std::chrono::system_clock::time_point &timePoint);

        std::chrono::system_clock::time_point msToTimePoint(std::int64_t ms);
    }
}