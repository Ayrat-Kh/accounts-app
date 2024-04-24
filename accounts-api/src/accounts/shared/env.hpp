#pragma once

#include <boost/process/env.hpp>

#include "accounts/shared/models.hpp"

namespace accounts
{
    const EnvironmentVar &getEnv();
}