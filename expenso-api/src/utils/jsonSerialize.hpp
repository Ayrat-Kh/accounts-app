#pragma once

#include <boost/json.hpp>
#include "utils/date.hpp"

namespace std::chrono
{

    boost::json::result<app::shared::Datetime> tag_invoke(boost::json::try_value_to_tag<app::shared::Datetime> v, const boost::json::value &s);
    void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, app::shared::Datetime timePoint);
}
