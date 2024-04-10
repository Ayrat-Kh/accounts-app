#pragma once

#include <boost/json.hpp>

#include "accounts/shared/types.hpp"

namespace std::chrono
{
    boost::json::result<::accounts::shared::Datetime> tag_invoke(boost::json::try_value_to_tag<::accounts::shared::Datetime> v, const boost::json::value &s);
    void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ::accounts::shared::Datetime timePoint);
}
