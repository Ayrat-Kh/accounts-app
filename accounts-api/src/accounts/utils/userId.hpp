#pragma once

#include <string_view>

namespace accounts
{
    inline bool isMe(std::string_view userId)
    {
        return userId == "me";
    }
}