#include "jsonSerialize.hpp"

namespace std::chrono
{
    void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, app::shared::Datetime timePoint)
    {
        jv = app::utils::timePointToString(timePoint);
    }
}
